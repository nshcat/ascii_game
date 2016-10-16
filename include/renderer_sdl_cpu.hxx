#pragma once

#include <vector>
#include <utility>
#include <unordered_map>
#include <SDL2/SDL.h>

#include "renderer_base.hxx"

namespace ascii
{
	auto make_rect(position p_pos, dimensions p_dim) noexcept
		-> SDL_Rect;
	
	
	
	class renderer_sdl_cpu final
		: public renderer_base
	{
		constexpr static std::size_t fps_filter_length = 12;
		
		public:
			virtual ~renderer_sdl_cpu() override
			{
				
			}
		
		public:
			virtual double fps() const noexcept override;
			virtual ut::string_view str() const noexcept override;
			virtual void create(const renderer_params& p_params) override;
			virtual void destroy() override;
			virtual void begin_scene() override;
			virtual void end_scene() override;
			
			virtual void update_dimensions(dimensions p_val) override;
			virtual void update_title(ut::string_view p_str) override;
			
		public:
			virtual void put_glyph(position p_pos, glyph_type p_glyph, color p_front, color p_back) override;
			virtual void put_shadow(position p_pos) override;
			
		private:
			auto present_screen()
				-> void;
		
			auto fetch_glyph_tex(const glyph_data& p_glyph)
				-> SDL_Texture*;
		
			auto create_glyph_tex(const glyph_data& p_glyph)
				-> SDL_Texture*;
		
		private:
			auto init_fps_filter() noexcept
				-> void;
		
			auto init_framebuffer()
				-> void;			
					
			auto set_params(const renderer_params& p_params)
				-> void;
				
			auto canonicalize_format(SDL_Surface* src) const
				-> SDL_Surface*;
				
			auto load_texture(const renderer_params& p_params)
				-> void;		
		
		private:		
			[[noreturn]]
			auto report_sdl_error(const char* p_str, bool p_isImg = false) const
				-> void;
			
			auto report_init(const renderer_params& p_params) const
				-> void;
				
		private:
			std::unordered_map<glyph_data, SDL_Texture*> m_TexCache;	// Texture cache
			
			
			std::vector<glyph_data> m_Screen; 		// Screen framebuffer
			std::vector<position> m_Coords;			// Screen space coordinates for
													// each framebuffer entry
			
			/* SDL handles */
			SDL_Renderer* m_Renderer;				// SDL renderer handle
			SDL_Window* m_Window;					// SDL window handle
			SDL_Surface* m_GlyphSurface;			// Surface containing glyph sheet
		    /* */
		
			/* Renderer parameters */
			std::size_t m_GlyphsX;					// Texture width in glyphs
			std::size_t m_GlyphsY;					// Texture height in glypghs
			std::size_t m_GlyphW;					// Glyph width in pixels
			std::size_t m_GlyphH;					// Glyph height in pixels
			std::size_t m_ScrW;						// Screen width in glyphs
			std::size_t m_ScrH;						// Screen height in glyphs
			/* */
		
		
			const char m_Str[8]{ "sdl_cpu" };		// Renderer identification string
			double m_Filter[fps_filter_length];		// Moving-average fps filter
	};
}
