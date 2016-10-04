#pragma once

#include <cstdint>
#include <tuple>

namespace ascii
{
	using glyph_type = std::uint16_t;
	
	class position
	{
		public:
			position() = default;
			
			position(std::size_t p_x, std::size_t p_y)
				: m_X(p_x), m_Y(p_y)
			{
				
			}
			
		public:
			auto x() const noexcept
				-> std::size_t
			{
				return m_X;
			}
			
			auto y() const noexcept
				-> std::size_t
			{
				return m_Y;
			}
		
		private:
			std::size_t m_X{};
			std::size_t m_Y{};
	};
		
	struct from_rgb_t{};
	struct from_hsv_t{};
	
	constexpr static from_rgb_t from_rgb {};
	constexpr static from_hsv_t from_hsv {};
	
	
	class color
	{
		using element_type = std::uint8_t;
		using element_tuple_type = std::tuple<element_type, element_type, element_type>;
				
		public:
			color(from_rgb_t, element_type p_r, element_type p_g, element_type p_b) noexcept
				: m_R(p_r), m_G(p_g), m_B(p_b)
			{
				
			}
			
			color(from_rgb_t, element_tuple_type p_vals) noexcept
				: m_R(std::get<0>(p_vals)), m_G(std::get<1>(p_vals)), m_B(std::get<2>(p_vals))
			{
				
			}
			
			color(from_hsv_t, element_type p_h, element_type p_s, element_type p_v) noexcept
			{
				std::tie(m_R, m_G, m_B) = to_rgb(p_h, p_s, p_v);
			}
			
			color(from_hsv_t, element_tuple_type p_vals) noexcept
			{
				std::tie(m_R, m_G, m_B) = to_rgb(std::get<0>(p_vals), std::get<1>(p_vals), std::get<2>(p_vals));
			}
		
		public:
			auto r() const noexcept
				-> element_type
			{
				return m_R;
			}
			
			auto g() const noexcept
				-> element_type
			{
				return m_G;
			}
			
			auto b() const noexcept
				-> element_type
			{
				return m_B;
			}
			
			auto rgb() const noexcept
				-> element_tuple_type
			{
				return std::make_tuple(m_R, m_G, m_B);
			}
				
			auto hsv() const noexcept
				-> element_tuple_type
			{
				return to_hsv(m_R, m_G, m_B);
			}
			
		private:
			auto to_rgb(element_type p_h, element_type p_s, element_type p_h) noexcept
				-> element_tuple_type;
				
			auto to_hsv(element_type p_r, element_type p_g, element_type p_b) noexcept
				-> element_tuple_type;
		
		private:
			element_type m_R;
			element_type m_G;
			element_type m_B;
	};	
}
