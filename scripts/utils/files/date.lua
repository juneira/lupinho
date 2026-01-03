local uv = require('luv')

local function get_precise_time_from_file(data)
    local filepath = data.path
    local stat, err = uv.fs_stat(filepath)
    if not stat then
        error("Failed to get file attributes: " .. err)
    end

    local mod_sec = stat.mtime.sec
    local formatted_time = os.date("%Y%m%d%H%M%S", mod_sec)
    return formatted_time
end

local function get_time_components_from_precise_time(unique_id)
    local year = tonumber(unique_id:sub(1, 4))
    local month = tonumber(unique_id:sub(5, 6))
    local day = tonumber(unique_id:sub(7, 8))
    local hour = tonumber(unique_id:sub(9, 10))
    local min = tonumber(unique_id:sub(11, 12))
    local sec = tonumber(unique_id:sub(13, 14))
    local index = tonumber(unique_id:sub(15)) or 0
    local time = os.time({year = year, month = month, day = day, hour = hour, min = min, sec = sec})
    return time, index
end

return {
    get_precise_time_from_file = get_precise_time_from_file,
    get_time_components_from_precise_time = get_time_components_from_precise_time
}
