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
		element_type t_rgbMin, t_rgbMax;
		element_type t_h, t_s, t_v;
	
		t_rgbMin = p_r < p_g ? (p_r < p_b ? p_r : p_b) : (p_g < p_b ? p_g : p_b);
		t_rgbMax = p_r > p_g ? (p_r > p_b ? p_r : p_b) : (p_g > p_b ? p_g : p_b);

		t_v = t_rgbMax;		
		if(t_v == 0)
		{
			return std::make_tuple(t_v, t_v, t_v);
		}
		
		t_s = 255 * ((long)t_rgbMax - t_rgbMin) / t_v;
		if (t_s == 0)
		{
			return std::make_tuple(t_s, t_s, t_v);
		}
		
		if (t_rgbMax == p_r)
			t_h = 0 + 43 * (p_g - p_b) / (t_rgbMax - t_rgbMin);
		else if (t_rgbMax == p_g)
			t_h = 85 + 43 * (p_b - p_r) / (t_rgbMax - t_rgbMin);
		else
			t_h = 171 + 43 * (p_r - p_g) / (t_rgbMax - t_rgbMin);
		
		return std::make_tuple(t_h, t_s, t_v);
	}
}