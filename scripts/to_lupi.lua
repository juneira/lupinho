#!/usr/bin/env lua

local uv = require("luv")
local argparse = require("argparse")
local tiled_encoder = require("encoders.lua_from_tiled_encoder")
local bitmap_encoder = require("encoders.bitmap_encoder")

require("utils.table.dump")

local SpriteSheets = {}

local function encode_all_files(path)
    local req = uv.fs_scandir(path)
    while true do
        local name, type = uv.fs_scandir_next(req)
        if not name then break end
        local fullpath = path .. '/' .. name
        if type == "directory" then
            encode_all_files(fullpath)
        else
            local props = {
                file_name = name,
                path = fullpath,
                root_path = to_path
            }

            if name:match("%.tmj$") then
                tiled_encoder(props)

                local new_file_name = name:gsub("%.tmj$", ".lua")
                local file = io.open(to_path .. "/" .. new_file_name, "w")
                file:write(props.octet_content)
                file:close()
            end

            if name:match("%.png$") then
                bitmap_encoder(props)

                SpriteSheets[name:gsub("%.png$", "")] = {
                    data = props.octet_content,
                    width = props.extra_headers["x-tw"],
                    height = props.extra_headers["x-th"],
                    ntiles = props.octet_content:len() / (props.extra_headers["x-tw"] * props.extra_headers["x-th"])
                }
            end
        end
    end
end

local parser = argparse("to_lupi", "Convert Tiled maps to Lupi format")
parser:option("-f, --from", "From path")
parser:option("-t, --to", "To path")
local args = parser:parse()

from_path = args.from
to_path = args.to

os.execute("mkdir -p " .. to_path)
os.execute("cp -r " .. from_path .. "/* " .. to_path)

encode_all_files(to_path)

local file = io.open(to_path .. "/sprites.lua", "w")
file:write("SpriteSheets = " .. table.dump(SpriteSheets, -1, false))
file:close()
