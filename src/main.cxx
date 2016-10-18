#include <iostream>
#include <string>
#include <log.hxx>
#include <cl.hxx>
#include <sol.hpp>
#include <ut/utility.hxx>

#include <renderer_sdl_cpu.hxx>

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
	}
};


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
	
	// Create logger
	ascii::renderer_params t_info{
		ascii::dimensions{ 50, 50 },
		"app",
		g_Tex
	};
	
	ascii::renderer_sdl_cpu t_renderer{ };
	
	t_renderer.create(t_info);
	
	while(true)
	{
		SDL_Event t_event;
		
		while(SDL_PollEvent(&t_event))
		{
			switch(t_event.type)
			{
				case SDL_QUIT:
					return EXIT_SUCCESS;
					break;
				
				default:
					break;
			}
		}
		
		t_renderer.begin_scene();
		
		for(int j = 0; j < 50; ++j)
			for(int i = 0; i < 50; ++i)
			{
				t_renderer.put_glyph({i, j}, (i%16)+16*(j%16), { ascii::from_hsv, i*5, 255, 255 }, { ascii::from_hsv, 0, 0, 0 });
			}
			
		t_renderer.end_scene();
	}
	
	return EXIT_SUCCESS;
}