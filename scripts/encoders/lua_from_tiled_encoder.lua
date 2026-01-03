local json = require("cjson")
local bit = require("bit")

local function tiled_id_to_lupi_id(gid)
    local flipped_horizontally  = bit.band(gid, 0x80000000) ~= 0
    local flipped_vertically    = bit.band(gid, 0x40000000) ~= 0
    local tile_id               = bit.band(gid, 0x1FFFFFFF)

    -- lupi id is 10 bits for tile, 1 bit for flip x and 1 bit for flip y
    return (tile_id % 112) + (flipped_horizontally and 1024 or 0) + (flipped_vertically and 2048 or 0)
end

local function encode_tiled_to_lua(content)
    local response = {}
    local json_data = json.decode(content)
    local tiles_layer, colision_layer, pois_layer, overlay_layer

    for _, layer in ipairs(json_data.layers) do
        if layer.name == 'tiles' then
            tiles_layer = layer
        elseif layer.name == 'colision' then
            colision_layer = layer
        elseif layer.name == 'pois' then
            pois_layer = layer
        elseif layer.name == 'overlay' then
            overlay_layer = layer
        end
    end

    local width = tiles_layer.width
    local height = tiles_layer.height

    for y = 1, height do
        response[y] = {}
        for x = 1, width do
            local tile_index = (y - 1) * width + (x - 1)
            local tile_value = tiles_layer.data[tile_index + 1]
            local colision_value = colision_layer and colision_layer.data[tile_index + 1] or nil
            local poi_value = pois_layer and pois_layer.data[tile_index + 1] or nil
            local overlay_value = overlay_layer and overlay_layer.data[tile_index + 1] or nil

            tile_id = tiled_id_to_lupi_id(tile_value)
            if overlay_value then overlay_value = tiled_id_to_lupi_id(overlay_value) end

            response[y][x] = {
                t = tile_id,
                c = (colision_value and colision_value ~= 0) and colision_value or nil,
                p = (poi_value and poi_value ~= 0) and poi_value or nil,
                o = (overlay_value and overlay_value ~= 0) and overlay_value or nil,
            }
        end
    end

    return response
end

local function generate_lua_table_string(lua_table)
    local lua_table_string = "return{\n"
    for y, row in ipairs(lua_table) do
        lua_table_string = lua_table_string .. "[" .. y .. "]={"
        for x, cell in ipairs(row) do
            local tile_id = bit.band(cell.t, 0x3FF)
            local poi_id  = cell.p and bit.band(cell.p, 0x3FF) or nil
            local overlay_id = cell.o and bit.band(cell.o, 0x3FF) or nil
            if (tile_id ~= 0 and tile_id ~= nil) or (poi_id ~= 0 and poi_id ~= nil) then
                if cell.t == 0 or cell.t == nil then
                    cell.t = "nil"
                end

                lua_table_string = lua_table_string .. "[" .. x .. "]={" .. cell.t

                if cell.c ~= nil then
                    lua_table_string = lua_table_string .. ",[2]=" .. cell.c
                end

                if poi_id then
                    lua_table_string = lua_table_string .. ",[3]=" .. cell.p
                end

                if overlay_id then
                    lua_table_string = lua_table_string .. ",[4]=" .. cell.o
                end

                lua_table_string = lua_table_string .. "},"
            end
        end
        lua_table_string = lua_table_string .. "},"
    end
    lua_table_string = lua_table_string .. "}"
    return lua_table_string
end

return function(props)
    local file_name, path = props.file_name, props.path
    local file = io.open(path, "r")

    if not file then
        error("File not found: " .. path)
    end

    local content = file:read("*all")
    file:close()

    local lua_table = encode_tiled_to_lua(content)
    local lua_table_string = generate_lua_table_string(lua_table)
    props.octet_content = lua_table_string
    props.sendable = true
    props.type = "lua_code"
    props.file_name = file_name .. ".lua"
end
