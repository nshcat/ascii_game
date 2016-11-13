function put_glyph(glyph, posx, posy, clr1, clr2)
	put_glyph_raw(glyph, posx, posy, clr1.r, clr1.g, clr1.b, clr2.r, clr2.g, clr2.b)
end

function put_str(str, posx, posy, clr1, clr2)
	for i = 1, #str do
		local c = str:sub(i,i)
		local glyph = string.byte(c)
		put_glyph(glyph, posx + i - 1, posy, clr1, clr2)
	end
end