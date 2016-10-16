#pragma once

#include <functional>
#include <cstdint>
#include <tuple>

#include <ut/hash.hxx>

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
			
			auto w() const noexcept
				-> std::size_t
			{
				return m_X;
			}
			
			auto h() const noexcept
				-> std::size_t
			{
				return m_Y;
			}
		
		private:
			std::size_t m_X{};
			std::size_t m_Y{};
	};
	
	using dimensions = position;
		
	struct from_rgb_t{};
	struct from_hsv_t{};
	
	constexpr static from_rgb_t from_rgb {};
	constexpr static from_hsv_t from_hsv {};
	
	
	class color
	{
		using element_type = std::uint8_t;
		using element_tuple_type = std::tuple<element_type, element_type, element_type>;
				
		public:
			color()
				: m_R{}, m_G{}, m_B{}
			{
				
			}
			
		
			color(from_rgb_t, element_type p_r, element_type p_g, element_type p_b) noexcept
				: m_R(p_r), m_G(p_g), m_B(p_b)
			{
				
			}
			
			color(from_rgb_t, element_tuple_type p_vals) noexcept
			{
				std::tie(m_R, m_G, m_B) = p_vals;
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
		
		public:
			auto operator==(const color& p_rhs) const noexcept
				-> bool
			{
				return rgb() == p_rhs.rgb();
			}
			
			auto operator!=(const color& p_rhs) const noexcept
				-> bool
			{
				return !(*this == p_rhs);
			}
			
		private:
			auto to_rgb(element_type p_h, element_type p_s, element_type p_v) const noexcept
				-> element_tuple_type;
				
			auto to_hsv(element_type p_r, element_type p_g, element_type p_b) const noexcept
				-> element_tuple_type;
		
		private:
			element_type m_R;
			element_type m_G;
			element_type m_B;
	};
	
	
	
	class glyph_data
	{
		public:
			glyph_data()
				: m_Glyph{}, m_Front{}, m_Back{}
			{
				
			}
		
			glyph_data(glyph_type p_glyph, color p_front, color p_back) noexcept
				: m_Glyph(p_glyph), m_Front(p_front), m_Back(p_back)
			{
				
			}
		
		public:
			auto operator==(const glyph_data& p_rhs) const noexcept
				-> bool
			{
				return std::tie(m_Glyph, m_Front, m_Back)
					== std::tie(p_rhs.m_Glyph, p_rhs.m_Front, p_rhs.m_Back);
			}
			
			auto operator!=(const glyph_data& p_rhs) const noexcept
				-> bool
			{
				return !(*this == p_rhs);
			}
		
		public:
			auto glyph() const noexcept
				-> glyph_type
			{
				return m_Glyph;
			}
			
			auto front() const noexcept
				-> color
			{
				return m_Front;
			}
			
			auto back() const noexcept
				-> color
			{
				return m_Back;
			}
			
			auto empty() const noexcept
				-> bool
			{
				return (*this == glyph_data{});
			}
			
		public:
			auto clear() noexcept
				-> void
			{
				m_Glyph = {};
				m_Front = m_Back = color(from_rgb, 0, 0, 0);
			}
		
		private:
			glyph_type m_Glyph;
			color m_Front;
			color m_Back;
	};
}

namespace std
{
	template<>
	struct hash <::ascii::color>
	{
		using argument_type = ::ascii::color;
		using result_type = size_t;
		
		auto operator()(const argument_type& p_val) const
			-> result_type
		{
			auto t_seed = ut::make_hash(p_val.r());
			
			ut::hash_combine(t_seed, p_val.g());
			ut::hash_combine(t_seed, p_val.b());
			
			return t_seed;
		}
	};
	
	template<>
	struct hash<::ascii::glyph_data>
	{
		using argument_type = ::ascii::glyph_data;
		using result_type = size_t;
		
		auto operator()(const argument_type& p_val) const
			-> result_type
		{
			auto t_seed = ut::make_hash(p_val.glyph());
			
			ut::hash_combine(t_seed, p_val.front());
			ut::hash_combine(t_seed, p_val.back());
			
			return t_seed;
		}
	};
}
