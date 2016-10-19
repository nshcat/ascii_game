#include <algorithm>
#include <iterator>
#include <limits>

#include <SDL2/SDL_image.h>

#include <log.hxx>

#include <renderer_sdl_cpu.hxx>

namespace ascii
{
	auto make_rect(position p_pos, dimensions p_dim) noexcept
		-> SDL_Rect
	{
		// TODO for now, we do not do any range checking here since
		// this could potential be a hot function.
		return SDL_Rect{
			static_cast<int>(p_pos.x()),
			static_cast<int>(p_pos.y()),
			static_cast<int>(p_dim.w()),
			static_cast<int>(p_dim.h())
		};	
	}
	
	auto renderer_sdl_cpu::str() const noexcept 
		-> ut::string_view
	{
		return { m_Str };
	}
	
	auto renderer_sdl_cpu::ticks() const noexcept
		-> std::size_t
	{
		return SDL_GetTicks();
	}

	auto renderer_sdl_cpu::destroy()
		-> void
	{
		// Free SDL resources
		for(auto& t_tex: m_TexCache)
			SDL_DestroyTexture(t_tex.second);
		
		SDL_FreeSurface(m_GlyphSurface);
		SDL_DestroyRenderer(m_Renderer);
		SDL_DestroyWindow(m_Window);
		
		IMG_Quit();
		SDL_Quit();
		//
	}

	auto renderer_sdl_cpu::begin_scene()
		-> void
	{
		// Clear window
		SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 0);
		SDL_RenderClear(m_Renderer);
	}

	auto renderer_sdl_cpu::make_index(position p_pos) const noexcept
		-> std::size_t
	{
		return (p_pos.y() * m_ScrW) + p_pos.x();
	}
	
	auto renderer_sdl_cpu::end_scene()
		-> void
	{
		// Present modified framebuffer
		present_screen();
		
		// Switch buffers
		SDL_RenderPresent(m_Renderer);
	}

	auto renderer_sdl_cpu::put_string(position p_pos, ut::string_view p_str, color p_front, color p_back)
		-> void
	{
		for(std::size_t t_idx = 0; t_idx < p_str.length(); ++t_idx)
		{
			put_glyph_impl({ p_pos.x() + t_idx, p_pos.y() }, p_str[t_idx], p_front, p_back);
		}	
	}
	
	auto renderer_sdl_cpu::put_glyph(position p_pos, glyph_type p_glyph, color p_front, color p_back)
		-> void
	{
		put_glyph_impl(p_pos, p_glyph, p_front, p_back);
	}
	
	auto renderer_sdl_cpu::put_glyph_impl(position p_pos, glyph_type p_glyph, color p_front, color p_back)
		-> void
	{
		// Calculate index into framebuffer
		auto t_index = make_index(p_pos);
		
		// Create glyph data entry
		m_Screen[t_index] = glyph_data{ p_glyph, p_front, p_back };
	}

	auto renderer_sdl_cpu::create(const renderer_params& p_params)
		-> void
	{
		// Print debug information about requested initialization
		report_init(p_params);
		
		// Set internal renderer parameters
		set_params(p_params);

		
		// Init SDL2
		if(SDL_Init(SDL_INIT_VIDEO) < 0)
			report_sdl_error("Failed to initialize SDL2!");
		//
		
		
		// Init SDL_Image
		auto t_reqflags = IMG_INIT_JPG | IMG_INIT_PNG;
		
		if((IMG_Init(t_reqflags)&t_reqflags) != t_reqflags)
			report_sdl_error("Failed to initialize SDL_Image!", true);
		//
		
		
		// Create window. The starting dimensions aren't important for now.
		m_Window = SDL_CreateWindow(
			p_params.title().c_str(),	// Window title
			SDL_WINDOWPOS_CENTERED,		// Spawn window in middle of screen
			SDL_WINDOWPOS_CENTERED,
			20,							// Starting dimensions are not
			20,							// important. They will be changed later.
			SDL_WINDOW_SHOWN			// Display window.
		);
		
		if(!m_Window)
			report_sdl_error("Failed to create SDL main window!");
		//
		
		
		// Create renderer
		m_Renderer = SDL_CreateRenderer(
			m_Window,					// Use main window as render target
			-1,
			SDL_RENDERER_ACCELERATED |
			SDL_RENDERER_TARGETTEXTURE |
			(p_params.vsync() ? SDL_RENDERER_PRESENTVSYNC : 0)		
		);
		
		if(!m_Renderer)
			report_sdl_error("Failed to create SDL renderer!");
		//
		

		// Load glyph sheet texture
		load_texture(p_params);
		
		// Initialize framebuffer
		init_framebuffer();
	}

	auto renderer_sdl_cpu::update_dimensions(dimensions p_val)
		-> void
	{
		// Set internal state to new values
		m_ScrH = p_val.h();
		m_ScrW = p_val.w();
		
		// Update framebuffer
		init_framebuffer();
		
		// Resize window accordingly
		set_window_dims();
	}
	
	auto renderer_sdl_cpu::update_title(const std::string& p_str)
		-> void
	{
		SDL_SetWindowTitle(m_Window, p_str.c_str());
	}

	auto renderer_sdl_cpu::init_framebuffer()
		-> void
	{
		// We want exactly W * H entries
		m_Screen.resize(m_ScrW * m_ScrH);
		m_Coords.resize(m_ScrW * m_ScrH);
		
		// Clear elements
		for(auto& t_elem: m_Screen)
			t_elem.clear();
		
		// Compute coordinates
		for(std::size_t t_iy = 0; t_iy < m_ScrH; ++t_iy)
		{
			for(std::size_t t_ix = 0; t_ix < m_ScrW; ++t_ix)
			{
				const auto t_index = (t_iy * m_ScrW) + t_ix;

				m_Coords[t_index] = position{ t_ix * m_GlyphW, t_iy * m_GlyphH };
			}
		}
	}

	auto renderer_sdl_cpu::put_shadow(position p_pos)
		-> void
	{
		// Compute index into framebuffer
		auto t_index = make_index(p_pos);
		
		// Retrieve glyph data
		auto& t_data = m_Screen[t_index];
		
		// Darken colours by 50%
		auto t_bg = t_data.back().hsv();
		auto t_fg = t_data.front().hsv();
		
		std::get<2>(t_bg) *= 0.5;
		std::get<2>(t_fg) *= 0.5;
		//
		
		// Save back to frame buffer
		t_data = glyph_data{ t_data.glyph(), { from_hsv, t_fg }, { from_hsv, t_bg } };		
	}
	
	auto renderer_sdl_cpu::load_texture(const renderer_params& p_params)
		-> void
	{
		// Load texture file
		SDL_Surface* t_img = IMG_Load(p_params.texture_path().c_str());
		
		if(!t_img)
			report_sdl_error("Failed to load glyph texture!", true);
		//
		
		// Convert surface to fixed pixel format
		m_GlyphSurface = canonicalize_format(t_img);
		
		// Calculate dimensions of glyph and screen in pixels
		m_GlyphW = m_GlyphSurface->w/m_GlyphsX;
		m_GlyphH = m_GlyphSurface->h/m_GlyphsY;
		
		// Update actual screen size
		set_window_dims();
	}
	
	auto renderer_sdl_cpu::set_window_dims()
		-> void
	{
		auto t_scrW = m_GlyphW * m_ScrW;
		auto t_scrH = m_GlyphH * m_ScrH;
		
		// Update main window screen size to fit
		SDL_SetWindowSize(m_Window, t_scrW, t_scrH);
	}

	auto renderer_sdl_cpu::canonicalize_format(SDL_Surface* p_src) const
		-> SDL_Surface*
	{
		// Create fixed pixel format used by the glyph sheet
		SDL_PixelFormat t_fmt;
		t_fmt.palette = NULL;
		t_fmt.BitsPerPixel = 32;
		t_fmt.BytesPerPixel = 4;
		t_fmt.Rloss = t_fmt.Gloss = t_fmt.Bloss = t_fmt.Aloss = 0;
		
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		t_fmt.Rshift = 24; t_fmt.Gshift = 16; t_fmt.Bshift = 8; t_fmt.Ashift = 0;
#else
		t_fmt.Rshift = 0; t_fmt.Gshift = 8; t_fmt.Bshift = 16; t_fmt.Ashift = 24;
#endif

		t_fmt.Rmask = 255 << t_fmt.Rshift;
		t_fmt.Gmask = 255 << t_fmt.Gshift;
		t_fmt.Bmask = 255 << t_fmt.Bshift;
		t_fmt.Amask = 255 << t_fmt.Ashift;
		//
		
		// Detect whether original texture had an alpha channel.
		// If not, interpret magenta as 0% alpha value.
		if(p_src->format->Amask == 0)
		{
			SDL_SetColorKey(p_src, SDL_TRUE, SDL_MapRGB(p_src->format, 255, 0, 255));
		}
		
		// Create new surface with fixed format and copy data
		SDL_Surface* t_dest = SDL_ConvertSurface(p_src, &t_fmt, SDL_SWSURFACE);
		
		if(!t_dest)
			report_sdl_error("Failed to canonicalize glyph surface!");
			
		// Cleanup
		SDL_FreeSurface(p_src);
		
		return t_dest;
	}

	auto renderer_sdl_cpu::present_screen()
		-> void
	{
		for(std::size_t t_index = 0; t_index < m_Screen.size(); ++t_index)
		{
			auto& t_data = m_Screen[t_index];
			
			if(!t_data.empty())
			{
				// Screen space coordinates of glyph, top left corner
				const auto t_pos = m_Coords[t_index];
				
				// Calculate destination
				auto t_destrect = make_rect(
					{ t_pos.x(), t_pos.y() },
					{ m_GlyphW, m_GlyphH }
				);
				
				// Copy to screen
				SDL_RenderCopy(
					m_Renderer,
					fetch_glyph_tex(t_data),
					NULL,
					&t_destrect
				);
				
				// Clear glyph
				t_data.clear();
			}
		}
	}

	auto renderer_sdl_cpu::create_glyph_tex(const glyph_data& p_glyph)
		-> SDL_Texture*
	{
		// Prepare values and temporary surfaces
		SDL_Surface* t_dest;
		
		auto t_srcx = m_GlyphW * (p_glyph.glyph() % m_GlyphsY);
		auto t_srcy = m_GlyphH * (p_glyph.glyph() / m_GlyphsX);
		
		
		// TODO range check!
		auto t_srcrect = make_rect(
			{ t_srcx, t_srcy },
			{ m_GlyphW, m_GlyphH }
		);
		
		auto t_destrect = make_rect(
			{ 0, 0 },
			{ m_GlyphW, m_GlyphH }
		);
		//
		
		
		// Create temporary surface
		auto t_fmt = m_GlyphSurface->format;
		t_dest = SDL_CreateRGBSurface(
			0,
			m_GlyphW,
			m_GlyphH,
			t_fmt->BitsPerPixel,
			t_fmt->Rmask,
			t_fmt->Gmask,
			t_fmt->Bmask,
			t_fmt->Amask
		);
		
		if(!t_dest)
			report_sdl_error("Failed to create temporary SDL surface for glyph texture!");
		//
		
		
		// Copy and color glyph
		SDL_BlitSurface(m_GlyphSurface, &t_srcrect, t_dest, &t_destrect);
		
		SDL_LockSurface(t_dest);
		
		
		float t_back_clr[3] = { p_glyph.back().r() / 255.f, p_glyph.back().g() / 255.f, p_glyph.back().b() / 255.f };
		float t_front_clr[3] = { p_glyph.front().r() / 255.f, p_glyph.front().g() / 255.f, p_glyph.front().b() / 255.f };
		
		for(std::size_t t_iy = 0; t_iy < t_dest->h; ++t_iy)
		{
			for(std::size_t t_ix = 0; t_ix < t_dest->w; ++t_ix)
			{
				std::uint8_t* t_pixel = (std::uint8_t*)t_dest->pixels + (t_dest->pitch * t_iy) + t_ix*4;
				
				float t_alpha = t_pixel[3] / 255.f;
				
				// For every color
				for(std::size_t t_i = 0; t_i < 3; ++t_i)
				{
					t_pixel[t_i] = ((t_alpha * t_front_clr[t_i]) + ((1 -  t_alpha) * t_back_clr[t_i])) * 255;
				}
				
				// Set full alpha
				t_pixel[3] = 255;
			}
		}
		
		SDL_UnlockSurface(t_dest);
		
		// /!\ IF SOMETHING BREAKS UNCOMMENT THIS
		//SDL_SetColorKey(t_dest, SDL_TRUE, SDL_MapRGB(t_fmt, 255, 0, 255));
		//
		
		SDL_Texture* t_tex = SDL_CreateTextureFromSurface(m_Renderer, t_dest);
		
		if(!t_tex)
			report_sdl_error("Failed to convert glyph surface to texture!");
		
		SDL_FreeSurface(t_dest);
		
		return t_tex;
	}

	auto renderer_sdl_cpu::fetch_glyph_tex(const glyph_data& p_data)
		-> SDL_Texture*
	{
		auto t_it = m_TexCache.find(p_data);
		
		if(t_it != m_TexCache.end())
		{
			return t_it->second;
		}
		else
		{
			auto t_tex = create_glyph_tex(p_data);
			m_TexCache[p_data] = t_tex;
			return t_tex;
		}
	}

	auto renderer_sdl_cpu::report_sdl_error(const char* p_str, bool p_isImg) const
		-> void
	{
		LOG_LOCK();
		LOG_F() << p_str;
		LOG_F_BARE() << "SDL2 error message:";
		LOG_F_BARE() << (p_isImg ? IMG_GetError() : SDL_GetError());
		LOG_UNLOCK();
		
		std::exit(EXIT_FAILURE);
	}

	auto renderer_sdl_cpu::report_init(const renderer_params& p_params) const
		-> void
	{
		LOG_LOCK();
		LOG_D() << "Renderer initialization requested:";
		LOG_D_BARE() << "\tRenderer:\t" << str();
		LOG_D_BARE() << "\tTexture:\t" << p_params.texture_path();
		LOG_D_BARE() << "\tGlyphs x:\t" << p_params.glyphs_x();
		LOG_D_BARE() << "\tGlyphs x:\t" << p_params.glyphs_x();
		LOG_D_BARE() << "\tWnd width:\t" << p_params.width();
		LOG_D_BARE() << "\tWnd height:\t" << p_params.height();
		LOG_D_BARE() << "\tUse vsync:\t" << std::boolalpha << p_params.vsync();
		LOG_D_BARE();
		LOG_UNLOCK();
	}

	auto renderer_sdl_cpu::set_params(const renderer_params& p_params)
		-> void
	{
		m_ScrW = p_params.width();
		m_ScrH = p_params.height();
		m_GlyphsX = p_params.glyphs_x();
		m_GlyphsY = p_params.glyphs_y();
	}
}