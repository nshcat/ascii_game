#pragma once

#include <ut/string_view.hxx>

#include "common.hxx"


namespace ascii
{
	class renderer_params
	{
		public:
			renderer_params(dimensions p_WndSz, dimensions p_Glyphs, std::string p_Title, std::string p_TexPath, bool p_Vsync)
				: 	m_WndW(p_WndSz.w()),
					m_WndH(p_WndSz.h()),
					m_Title(std::move(p_Title)),
					m_TexPath(std::move(p_TexPath)),
					m_GlyphsX(p_Glyphs.w()),
					m_GlyphsY(p_Glyphs.h()),
					m_Vsync(p_Vsync)
			{
				
			}
			
			renderer_params(dimensions p_WndSz, std::string p_Title, std::string p_TexPath, bool p_Vsync)
				: 	renderer_params(p_WndSz, { 16, 16 }, std::move(p_Title), std::move(p_TexPath), p_Vsync)
			{
				
			}
			
		public:
			auto width() const
				-> std::size_t
			{
				return m_WndW;
			}
			
			auto height() const
				-> std::size_t
			{
				return m_WndH;
			}
			
			auto title() const
				-> const std::string&
			{
				return m_Title;
			}
			
			auto texture_path() const
				-> const std::string&
			{
				return m_TexPath;
			}
			
			auto vsync() const
				-> bool
			{
				return m_Vsync;
			}
			
			auto glyphs_x() const
				-> std::size_t
			{
				return m_GlyphsX;
			}
			
			auto glyphs_y() const
				-> std::size_t
			{
				return m_GlyphsY;
			}
			
		private:
			std::size_t m_GlyphsX;	// Texture width, in glyphs
			std::size_t m_GlyphsY;	// Texture height, in glyphs
			std::size_t m_WndW;			// Window width in glyphs
			std::size_t m_WndH;			// Window height in glyphs
			std::string m_Title; 		// Window title
			std::string m_TexPath;		// Texture path
			bool m_Vsync;				// Whether to use vsync
			
	};
	
	
	class renderer_base
	{
		public:
			virtual ~renderer_base() = 0;
			
			
			// --- De-/Initialization ---
			virtual void create(const renderer_params& p_params) = 0;
			virtual void update_dimensions(dimensions p_val) = 0;
			virtual void update_title(const std::string& p_str) = 0;
			virtual void destroy() = 0;
			// ---
			
			
			// --- Non-mutable interface ---
			virtual ut::string_view str() const noexcept = 0;
			virtual std::size_t ticks() const noexcept = 0;
			// ---
			
			
			// --- Mutable interface
			virtual void begin_scene() = 0;
			virtual void end_scene() = 0;
			
			virtual void put_string(position p_pos, ut::string_view p_str, color p_front, color p_back) = 0;
			virtual void put_glyph(position p_pos, glyph_type p_glyph, color p_front, color p_back) = 0;
			virtual void put_shadow(position p_pos) = 0;
			// ---		
	};
	
	inline renderer_base::~renderer_base()
	{
		
	}
}