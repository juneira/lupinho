local colors = require("utils.bitmap.colors")
local png = require("decoders.png.png")
local to_png = require("utils.bitmap.to_png")
local pix_font = require("utils.bitmap.pix_font")

local DEFAULT_PALETTE_NAME = "Palette"

local function verify_png(image)
    if image.width * image.height > 512 * 96 then
        -- need to check after removing the margin
        --error("Wow! O tamanho da imagem excede o limite de 49 mil pixels\n - " .. image)
    end
end

local function save_palette_to_file(palette, path)
    -- Lua version of the palette
    local palette_str = "{"

    for i, color in ipairs(palette) do
        palette_str = palette_str .. string.format("0x%04X", color)
        if i < #palette then
            palette_str = palette_str .. ", "
        end
    end

    palette_str = palette_str .. "}"

    local source_code = DEFAULT_PALETTE_NAME .. " = " .. palette_str

    local file = io.open(path .. ".lua", "w")
    if not file then
        error("Failed to create palette file: " .. (err or "unknown error") .. " at path: " .. path .. ".lua")
    end

    file:write(source_code)
    file:close()

    -- PNG version of the palette
    local w, h = 478, 32 * 16
    local png_builder = to_png(w, h)

    for row = 1, h do
        for col = 1, w do

            local wrapper = math.floor(col / 60)
            local next_index = (wrapper * 32) + math.floor((row-1) / 16)

            if col > wrapper * 60 + 16 then
                local tx = (col - (wrapper * 60 + 16)) - 2
                local ty = (row-1) % 16 - 4
                local digit = nil

                if tx < 8 then
                    digit = "-"
                elseif tx < 16 then
                    local hundreds = math.floor(next_index / 100) % 10
                    digit = hundreds
                elseif tx < 24 then
                    local tens = math.floor(next_index / 10) % 10
                    digit = tens
                elseif tx < 32 then
                    local unit = next_index % 10
                    digit = unit
                end

                if digit and tx > 0 and ty > 0 and ty < 8 and pix_font[tostring(digit)][(ty % 8) + 1][(tx % 8) + 1] == 1 then
                    png_builder:write({205,205,205})
                else
                    png_builder:write({30,30,30})
                end

            elseif next_index <= #palette then
                local next_color = palette[1 + next_index % #palette]
                local r,g,b = colors.BGR555_to_RGB888_components(next_color)
                png_builder:write({r,g,b})
            else
                png_builder:write({0,0,0})
            end
        end
    end

    local png_bin = table.concat(png_builder.output)
    local png_file = assert(io.open(path .. ".png", "wb"))
    png_file:write(png_bin)
    png_file:close()
end

local function file_to_pixels(path)
    local image = png(path)
    verify_png(png(path))

    if not image then
        error("Image not found at: " .. path)
    end

    local rows = {}
    for py = 1, image.height do
        local row = {}
        for px = 1, image.width do
            local original_pixel = image:getPixel(px, py)
            local new_color = colors.RGB888_to_BGR555(original_pixel.R, original_pixel.G, original_pixel.B, original_pixel.A)
            table.insert(row, new_color)
        end
        table.insert(rows, row)
    end

    return rows
end

local function pixels_to_unique_colors(image_pixels)
    local unique_colors_builder = {}
    for _, row in ipairs(image_pixels) do
        for _, color in ipairs(row) do
            if not unique_colors_builder[color] then
                unique_colors_builder[color] = true
            end
        end
    end

    local unique_colors = { 0x00 }
    for color, _ in pairs(unique_colors_builder) do
        table.insert(unique_colors, color)
    end

    return unique_colors
end

local function load_and_check_current_palette(palette_path, unique_colors)
    _G[DEFAULT_PALETTE_NAME] = nil
    package.loaded[palette_path] = nil

    -- Use dofile for absolute paths, require for module paths
    local palette_file = palette_path .. ".lua"
    pcall( function() dofile(palette_file) end )
    local loaded_palette = _G[DEFAULT_PALETTE_NAME]

    if not loaded_palette or #loaded_palette == 0 then
        return "none"
    end

    local loaded_set, unique_set, missing_colors = {},{},{}

    for _, color in ipairs(loaded_palette) do
        loaded_set[color] = true
    end

    for _, color in ipairs(unique_colors) do
        unique_set[color] = true
    end

    for color in pairs(unique_set) do
        if not loaded_set[color] then
            table.insert(missing_colors, color)
        end
    end

    if #missing_colors > 0 then
        return "less", missing_colors
    elseif #loaded_palette > #unique_colors then
        return "more"
    else
        return "same"
    end
end

local function find_base_tile_sizing(image_pixels, magic_color)
    local w, h = #image_pixels[1], #image_pixels
    local base_tile_width, base_tile_height = w, h

    if image_pixels[1][1] == magic_color then
        for i = 1, w do
            if image_pixels[1][i] ~= magic_color then
                base_tile_width = i - 1
                break
            end
        end

        for j = 1, h do
            if image_pixels[j][1] ~= magic_color then
                base_tile_height = j - 1
                break
            end
        end
    end

    return base_tile_width, base_tile_height
end

local function encode_bitmap(image_pixels)
    local palette = _G[DEFAULT_PALETTE_NAME]
    local w, h = #image_pixels[1], #image_pixels
    local magic_gray = 8456 -- 0x424242 in RGB555
    local base_tile_width, base_tile_height = find_base_tile_sizing(image_pixels, magic_gray)
    local encoded_builder = ""

    -- Iterate over the image in tiles
    for tile_y = 0, math.floor(h / base_tile_height) - 1 do
        for tile_x = 0, math.floor(w / base_tile_width) - 1 do
            if (tile_x == 0 or tile_y == 0) and image_pixels[1][1] == magic_gray then
                -- dont need to encode
            else
                for row = 1, base_tile_height do
                    for col = 1, base_tile_width do
                        local color = image_pixels[tile_y * base_tile_height + row][tile_x * base_tile_width + col]
                        local index = 1

                        for i, c in ipairs(palette) do
                            if c == color then
                                index = i
                                break
                            end
                        end
                        encoded_builder = encoded_builder .. string.char(index - 1)
                    end
                end
            end
        end
    end

    return encoded_builder, base_tile_width, base_tile_height
end


local function touch_bitmap_files(path)
    print("\n [LGS] Touching bitmap files at " .. path .. "\n")
    local uv = require("luv")
    local to_be_touched = {}

    local function touch_file(file_path)
        print("\n[LGS] Touching file: " .. file_path .. "\n")

        local file, err = io.open(file_path, "r")
        if not file then
            print("[LGS] Failed to open for reading:", err)
            return false
        end

        local content = file:read("*a")
        file:close()

        local tmp_path = file_path .. ".tmp"
        local tmp_file
        tmp_file, err = io.open(tmp_path, "w")
        if not tmp_file then
            print("[LGS] Failed to open temp file for writing:", err)
            return false
        end

        tmp_file:write(content)
        tmp_file:close()

        local success, rename_err = os.rename(tmp_path, file_path)
        if not success then
            print("[LGS] Failed to rename temp file:", rename_err)
            return false
        end

        print("[LGS] Successfully touched file:", file_path)
        return true
    end

    local function recursive_touch(path)
        local req = uv.fs_scandir(path)
        while true do
            local name, type = uv.fs_scandir_next(req)
            if not name then break end
            local full_path = path .. "/" .. name
            if type == "directory" then
                recursive_touch(full_path)
            elseif name:match("%.png$") then
                table.insert(to_be_touched, full_path)
            end
        end
    end

    recursive_touch(path)

    for _, file_path in ipairs(to_be_touched) do
        touch_file(file_path)
    end
end

local function do_palette_review(can_use_current_palette, unique_colors, missing_colors, palette_path)
    print("\n [LGS] Palette review: " .. can_use_current_palette .. "\n")

    if can_use_current_palette == "same" or can_use_current_palette == "more" then
        -- nothing needed to be done
        return
    elseif can_use_current_palette == "none" then
        local game_path = palette_path:match("(.+)/[^/]+")
        touch_bitmap_files(game_path)
        save_palette_to_file(unique_colors, palette_path)
    elseif can_use_current_palette == "less" then

        local merged_palette = {}
        local color_set = {}

        for _, color in ipairs(_G[DEFAULT_PALETTE_NAME]) do
            table.insert(merged_palette, color)
            color_set[color] = true
        end

        for _, color in ipairs(missing_colors) do
            if not color_set[color] then
                table.insert(merged_palette, color)
                color_set[color] = true
            end
        end

        if #merged_palette > 256 then
            if #unique_colors > 256 then
                error("\n [LGS] Failed to generate palette! TOO MANY COLORS \n")
            else
                print("\n [LGS] Resetting Palette with " .. #unique_colors .. " colors \n")
                save_palette_to_file(unique_colors, palette_path)
                -- palette_path ends with /palette, so remove last path component to get the game path
                local game_path = palette_path:match("(.+)/[^/]+")
                touch_bitmap_files(game_path)
            end
        else
            -- just adding more colors should be compatible with
            -- others images, already indexed or to be indexed
            save_palette_to_file(merged_palette, palette_path)
        end
    else
        error("not implemented")
    end

    local can_use_current_palette = load_and_check_current_palette(palette_path, unique_colors)
    if can_use_current_palette == "less" or can_use_current_palette == "none" then
        error("\n [LGS] Failed to generate palette for status: " .. can_use_current_palette .. "\n")
    end
end

return function(props)
    local file_name, path, root_path = props.file_name, props.path, props.root_path

    local image_pixels = file_to_pixels(path)
    local unique_colors = pixels_to_unique_colors(image_pixels)

    local palette_path = root_path .. "/palette"
    local can_use_current_palette, missing_colors = load_and_check_current_palette(palette_path, unique_colors)
    do_palette_review(can_use_current_palette, unique_colors, missing_colors, palette_path)

    local sh, sw = 0, 0
    props.octet_content, sw, sh = encode_bitmap(image_pixels)
    props.sendable = #props.octet_content > 0
    props.extra_headers = {
        ["x-tw"] = sw,
        ["x-th"] = sh
    }
end
