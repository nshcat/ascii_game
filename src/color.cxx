#include <ut/utility.hxx>

#include <common.hxx>

namespace ascii
{
	// Source: http://web.mit.edu/storborg/Public/hsvtorgb.c
	auto color::to_rgb(element_type p_h, element_type p_s, element_type p_v) const noexcept
		-> element_tuple_type
	{
		element_type t_region, t_fpart, t_p, t_q, t_t;
		
		// Grayscale color
		if(p_s == 0)
		{
			return std::make_tuple(p_v, p_v, p_v);
		}
		
		// Transform hue to be [0, 5]
		t_region = p_h / 43;
		// Remainder part, [0, 255]
		t_fpart = (p_h - (t_region * 43)) * 6;
		
		// Calculate temporary values
		t_p = (p_v * (255 - p_s)) >> 8;
		t_q = (p_v * (255 - ((p_s * t_fpart) >> 8))) >> 8;
		t_t = (p_v * (255 - ((p_s * (255 - t_fpart)) >> 8))) >> 8;
		
		// Determine r g b values based on color cone region
		switch(t_region)
		{
			case 0:
				return std::make_tuple(p_v, t_t, t_p);
				break;
			
			case 1:
				return std::make_tuple(t_q, p_v, t_p);
				break;
				
			case 2:
				return std::make_tuple(t_p, p_v, t_t);
				break;
				
			case 3:
				return std::make_tuple(t_p, t_q, p_v);
				break;
				
			case 4:
				return std::make_tuple(t_t, t_p, p_v);
				break;
				
			default:
				return std::make_tuple(p_v, t_p, t_q);
				break;
		}
	}

	// Source: https://www.ruinelli.ch/rgb-to-hsv
	auto color::to_hsv(element_type p_r, element_type p_g, element_type p_b) const noexcept
		-> element_tuple_type
	{
		std::uint16_t t_h;
		element_type t_v;
		element_type t_s;
		element_type t_min, t_max, t_delta;
		
		if(p_r < p_g)
			t_min = p_r;
		else
			t_min = p_g;
		
		
		if(p_r > p_g)
			t_max = p_r;
		else
			t_max = p_g;
		
		
		if(p_b > t_max)
			t_max = p_b;
		
		
		t_v = t_max;
		t_delta = t_max - t_min;
		
		
		if(t_max != 0)
			t_s = static_cast<element_type>((t_delta*255) / t_max);
		else
		{
			return std::make_tuple(element_type{}, element_type{}, element_type{});
		}
		
		
		if(t_max == p_r)
			t_h = (p_g - p_b) * 60 / t_delta;
		else if(t_max == p_g)
			t_h = 120 + (p_b - p_r) * 60 / t_delta;
		else
			t_h = 240 + (p_r - p_g) * 60 / t_delta;
		
		if(t_h < 0)
			t_h += 360;
		
		return std::make_tuple(ut::lmap(t_h, std::uint16_t{}, std::uint16_t{360}, element_type{0}, element_type{255}), t_s, t_v);	
	}
}