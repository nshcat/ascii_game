#pragma once

#include <ut/string_view.hxx>

#include "common.hxx"

namespace ascii
{	
	class renderer_params
	{
		public:
			renderer_params(std::size_t p_WndW, std::size_t p_WndH, ut::string_view p_Title, ut::string_view p_TexPath)
				: m_WndW(p_WndW), m_WndH(p_WndH), m_Title(p_Title), m_TexPath(p_TexPath)
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
				-> ut::string_view
			{
				return m_Title;
			}
			
			auto texture_path() const
				-> ut::string_view
			{
				return m_TexPath;
			}
			
			
		private:
			std::size_t m_WndW;			// Window width in glyphs
			std::size_t m_WndH;			// Window height in glyphs
			ut::string_view m_Title; 	// Window title
			ut::string_view m_TexPath;	// Texture path
			
	};
	
	
	class renderer_base
	{
		virtual ~renderer_base() = 0;
		
		
		// --- De-/Initialization ---
		virtual void create(const renderer_params& p_params) = 0;
		virtual void destroy() = 0;
		// ---
		
		s
		// --- Non-mutable interface ---
		virtual ut::string_view str() const noexcept = 0;
		virtual double fps() const noexcept = 0;
		// ---
		
		
		// --- Mutable interface
		virtual void begin_scene() = 0;
		virtual void end_scene() = 0;
		
		virtual void put_glyph(glyph_type p_glyph, position p_pos, color p_front, color p_back) = 0;
		virtual void put_shadow(position p_pos) = 0;
		// ---
		
	};
	
	inline renderer_base::~renderer_base()
	{
		
	}
}