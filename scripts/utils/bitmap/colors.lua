

return {
    Hex_to_BGR555 = function(RGB888_color)
        local r = bit.rshift(bit.band(RGB888_color, 0xFF0000), 16)
        local g = bit.rshift(bit.band(RGB888_color, 0x00FF00), 8)
        local b = bit.band(RGB888_color, 0x0000FF)
        return bit.bor(bit.lshift(b, 10), bit.lshift(g, 5), r)
    end,
    RGB888_to_BGR555 = function(r, g, b, a)
        if a == 255 then
            r = bit.rshift(r, 3)
            g = bit.rshift(g, 3)
            b = bit.rshift(b, 3)
            return bit.bor(bit.lshift(b, 10), bit.lshift(g, 5), r)
        else
            return 0
        end 
    end,
    BGR555_to_hex = function(color)
        return string.format("0x%04X", color)
    end,
    BGR555_to_RGB888_components = function(color)
        local r = bit.band(bit.rshift(color, 0), 0x1F)
        local g = bit.band(bit.rshift(color, 5), 0x1F)
        local b = bit.band(bit.rshift(color, 10), 0x1F)
        r = bit.lshift(r, 3)
        g = bit.lshift(g, 3)
        b = bit.lshift(b, 3)
        return r, g, b
    end
}