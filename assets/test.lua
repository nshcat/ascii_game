import("colors.lua")
import("draw.lua")
import("keys.lua")

local pos_x = 0
local pos_y = 0

function init()
	renderer_h = 25
	renderer_w = 25
	renderer_text = 'test_app'
end

function on_logic()
	if key_down(keys.esc) then
		quit(0)
	end
	
	if key_down(keys.up) then
		if pos_y ~= 0 then	
			pos_y = pos_y - 1
		end
	end
	
	if key_down(keys.down) then
		local max_y = renderer_h - 1
		if pos_y < max_y then	
			pos_y = pos_y + 1
		end
	end
	
	if key_down(keys.left) then
		if pos_x ~= 0 then	
			pos_x = pos_x - 1
		end
	end
	
	if key_down(keys.right) then
		local max_x = renderer_w - 1
		if pos_x < max_x then	
			pos_x = pos_x + 1
		end
	end
end

function on_frame()
	put_glyph(201, pos_x, pos_y, colors.green, colors.black)
	put_glyph(205, pos_x+1, pos_y, colors.green, colors.black)
	put_glyph(205, pos_x+2, pos_y, colors.green, colors.black)
	put_glyph(205, pos_x+3, pos_y, colors.green, colors.black)
	put_glyph(187, pos_x+4, pos_y, colors.green, colors.black)
end