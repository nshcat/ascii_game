#include <iostream>
#include <string>
#include <vector>
#include <log.hxx>
#include <cl.hxx>
#include <sol.hpp>
#include <ut/utility.hxx>

#include <sdl_input.hxx>
#include <renderer_factory.hxx>
#include <common.hxx>


// LUA state object
sol::state g_State;
// SDL input wrapper
proto::sdl_input g_Input;
// Renderer handler
ascii::renderer_base* g_RendererHandle;

// Variables referenced by command line handler ---
//

// Verbosity level of the logger
auto g_LogLevel = lg::severity_level::warning;

// Whether to log to file or not
bool g_LogToFile;

// Log file path
std::string g_LogFile;

// Texture file path
std::string g_Tex;

// Renderer identification
std::string g_Renderer;

// Script file path
std::string g_Script;

// Additional include files
std::vector<std::string> g_Includes;

// ------------------------------------------------


// Command line argument handler
cl::handler g_Handler
{
	cl::application_name("ascii_game"),
	cl::application_version("0.0"),
	cl::error_mode(cl::terminate),
	cl::print_help(false),
	
	cl::enum_argument<lg::severity_level>
	{
		cl::long_name("verbosity-level"),
		cl::ignore_case,
		
		cl::enum_key_value("all", 		lg::severity_level::debug),
		cl::enum_key_value("debug", 	lg::severity_level::debug),
		cl::enum_key_value("info", 		lg::severity_level::info),
		cl::enum_key_value("warning", 	lg::severity_level::warning),
		cl::enum_key_value("error", 	lg::severity_level::error),
		cl::enum_key_value("fatal", 	lg::severity_level::fatal),
		
		cl::reference(g_LogLevel)
	},
	
	cl::switch_argument<lg::severity_level>
	{
		cl::long_name("verbose"),
		cl::short_name('v'),
		
		cl::switch_value(lg::severity_level::debug),
		
		cl::reference(g_LogLevel)
	},
	
	cl::boolean_argument
	{
		cl::long_name("log-to-file"),
		cl::short_name('f'),
		cl::default_value(false),
		cl::reference(g_LogToFile)		
	},
	
	cl::string_argument
	{
		cl::long_name("log-file"),
		cl::default_value<std::string>("ascii_game.log"),
		cl::reference(g_LogFile)
	},
	
	cl::string_argument
	{
		cl::long_name("texture"),
		cl::short_name('T'),
		cl::default_value<std::string>(""),
		cl::reference(g_Tex)
	},
	
	cl::string_argument
	{
		cl::long_name("renderer"),
		cl::short_name('R'),
		cl::default_value<std::string>(""),
		cl::reference(g_Renderer)
	},
	
	cl::string_argument
	{
		cl::long_name("script"),
		cl::short_name('S'),
		cl::default_value<std::string>(""),
		cl::reference(g_Script)
	},
	
	cl::positional_argument<std::string>
	{
		cl::long_name("include-files"),
		cl::short_name('I'),
		cl::reference(g_Includes)
	}
};

void init_bindings()
{
	g_State["import"] = [](std::string module)
	{
		g_State.script_file(module);
	};
	
	g_State["quit"] = [](int code)
	{
		std::exit(code);
	};
	
	g_State["key_down"] = [](int key) -> bool
	{
		return g_Input.key_down((proto::sdl_input::key_type)key);
	};
	
	g_State["put_glyph_raw"] = [](	ascii::glyph_type c, std::size_t x, std::size_t y,
								std::uint8_t r1, std::uint8_t g1, std::uint8_t b1,
								std::uint8_t r2, std::uint8_t g2, std::uint8_t b2
							) -> void
	{
		g_RendererHandle->put_glyph({x,y}, c, {ascii::from_rgb, r1, g1, b1}, {ascii::from_rgb, r2, g2, b2});
	};
	
	g_State["put_shadow"] = [](std::size_t x, std::size_t y)
	{
		g_RendererHandle->put_shadow({x,y});
	};
}

void init_lua()
{
	g_State.open_libraries(sol::lib::base, sol::lib::string);
	
	// Set default renderer parameters
	g_State["renderer_vsync"] = true;
	g_State["renderer_w"] = 50;
	g_State["renderer_h"] = 50;
	g_State["renderer_text"] = std::string{"app"};
	
	// Initialize bindings
	init_bindings();
	
	// Load additional include files, if any
	for(const auto& t_file: g_Includes)
	{
		g_State.script_file(t_file);
	}
	
	// Load main script file
	g_State.script_file(g_Script);
}

ascii::renderer_params get_params()
{
	// Call init function
	g_State.get<std::function<void(void)>>("init")();
	
	// TODO bounds checks
	// Build renderer_params object
	return {
		{ g_State.get<std::size_t>("renderer_w"), g_State.get<std::size_t>("renderer_h") },
		g_State.get<std::string>("renderer_text"),
		g_Tex,
		g_State.get<bool>("renderer_vsync")
	};
}

int main(int argc, char* argv[])
{
	// Handle command line
	g_Handler.read(argc, (const char**)argv);

	// Init file target
	lg::file_target<> t_fileTarget(g_LogLevel, g_LogFile);
	
	// Initialize logger
	lg::logger::default_init(g_LogLevel);
	
	// Add file target if requested
	if(g_LogToFile)
	{	
		lg::logger::add_target(&t_fileTarget);
	}	
	
	// Request renderer object
	g_RendererHandle = ascii::create_instance(g_Renderer);
	
	if(!g_RendererHandle)
	{
		LOG_F() << "Unknown renderer plugin \"" << g_Renderer << "\"!";
		return EXIT_FAILURE;
	}
	//
	
	
	// Initialize LUA binding
	init_lua();
	//
	
	
	// Construct renderer
	ascii::renderer_params t_info = get_params();
	g_RendererHandle->create(t_info);
	//
	
	
	// Retrieve script function references
	auto t_onLogic = g_State.get<std::function<void(void)>>("on_logic");
	auto t_onFrame = g_State.get<std::function<void(void)>>("on_frame");
	//
	
	
	while(true)
	{
		// Process input
		g_Input.process_events();
		
		// Call on logic hook
		t_onLogic();
		
		g_RendererHandle->begin_scene();
		
		// Call on frame hook
		t_onFrame();
					
		g_RendererHandle->end_scene();
	}
	
	
	return EXIT_SUCCESS;
}