local json = require("cjson")
local bit = require("bit")

local function verify_map(map)
    if map.compressionlevel ~= -1 then
        error("Ops! Apenas mapas sem compressao (csv) sao suportados")
    end

    if map.infinite ~= false then
        error("Putz! Mapas infinitos nao sao suportados")
    end

    if map.orientation ~= "orthogonal" then
        error("Pera! Apenas mapas com orientacao ortogonal sao suportados")
    end

    if map.renderorder ~= "right-down" then
        error("Ave! Apenas mapas com render order right-down sao suportados")
    end

    if map.type ~= "map" then
        error("Nossa! Apenas mapas sao suportados")
    end
end

local function verify_layer(layer)
    if layer.type ~= "tilelayer" then
        error("Humm! Apenas camadas de tile sao suportadas")
    end

    if layer.x ~= 0 or layer.y ~= 0 then
        error("Poxa! O mapa precisa comecar em x = 0 e y = 0")
    end
end

local function verify_tileset(tileset)

    if tileset.tileheight ~= tileset.tilewidth or tileset.tileheight ~= tileset.margin then
        error(
            "Ah! Apenas tilesets com tileheight, tilewidth e margin iguais sao suportados" ..
            "\nOs tiles precisam ser quadrados e com margem configurada"
        )
    end

    if tileset.imagewidth * tileset.imageheight > 512 * 96 then
        error("Vixe! O tamanho da imagem do tileset excede o limite de 49 mil pixels")
    end

    if tileset.spacing ~= 0 then
        error("Ops! Apenas tilesets com spacing igual a zero sao suportados")
    end

    if not tileset.image:match("%.png$") then
        error("Puts! Apenas tilesets .png sao suportados")
    end

    if not tileset.image:match("^[%w_]+%.png$") or #tileset.image > 32 then
        error("Vish! O nome da imagem do tileset deve conter apenas letras, numeros\ne underline e ter menos de 24 caracteres")
    end
end

local function tiled_id_to_lupi_id(gid)
    local flipped_horizontally  = bit.band(gid, 0x80000000) ~= 0
    local flipped_vertically    = bit.band(gid, 0x40000000) ~= 0
    local rotated               = bit.band(gid, 0x20000000) ~= 0
    local tile_id               = bit.band(gid, 0x1FFFFFFF)
    return (tile_id) + (flipped_horizontally and 1024 or 0) + (flipped_vertically and 2048 or 0)
end

local function generate_lua_map(json_map)
    local map = json.decode(json_map)
    verify_map(map)

    local lua_map = {
        lupi_metadata = {
            width = map.width,
            height = map.height,
            tile_size = map.tilewidth
        }
    }


    for _, layer in ipairs(map.layers) do
        verify_layer(layer)
    end

    local tilesets = {}
    for _, tileset in ipairs(map.tilesets) do
        verify_tileset(tileset)

        local name = tileset.image:sub(1, #tileset.image - 4)
        local gid = tileset.firstgid
        table.insert (tilesets, {image = name, start = gid})
    end

    table.sort(tilesets, function(a, b) return a.start > b.start end)

    local tileset_for_gid = function(gid)
        for _, tileset in ipairs(tilesets) do
            if bit.band(gid, 0x3FF) >= tileset.start then
                return tileset
            end
        end

        error(string.format("Ops! GID %d nao encontrado", gid))
    end

    for _, layer in ipairs(map.layers) do
        local data = {}

        for i, gid in pairs(layer.data) do
            if gid > 0 then
                local tileset = tileset_for_gid(gid)
                local tile_id = tiled_id_to_lupi_id(gid - tileset.start)
                data[tileset.image] = data[tileset.image] or {}
                data[tileset.image][i] = tile_id
            end
        end

        table.insert(lua_map, {name = layer.name, data = data})
    end

    local output = "return {\n"

    for _, layer in ipairs(lua_map) do

        output = output .. "  [" .. string.format("%q", layer.name) .. "] = {\n"

        output = output .. "    lupi_metadata = {\n"
        output = output .. "      width = " .. lua_map.lupi_metadata.width .. ",\n"
        output = output .. "      height = " .. lua_map.lupi_metadata.height .. ",\n"
        output = output .. "      tile_size = " .. lua_map.lupi_metadata.tile_size .. ",\n"
        output = output .. "    },\n"

        for image, data in pairs(layer.data) do
            output = output .. "    [" .. string.format("%q", image) .. "] = {\n"
            for i, tile_id in pairs(data) do
                output = output .. "      [" .. i .. "] = " .. tile_id .. ",\n"
            end
            output = output .. "    },\n"
        end
        output = output .. "  },\n"
    end
    output = output .. "}\n"

    return output
end

return function(props)
    local file_name, path = props.file_name, props.path
    local file = io.open(path, "rb")

    if not file then
        error("File not found: " .. path)
    end

    local data = file:read("*all")
    file:close()

    props.octet_content = generate_lua_map(data)
    props.sendable = true
    props.type = "lua_code"
    props.file_name = file_name .. ".lua"
end
