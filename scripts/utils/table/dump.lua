local function table_dump_(value, indent, current_indent, try_tostring, done)
    if done[value] then -- to prevent recursive
        return "[recursive node]"
    end
    if type(value) == "table" then
        if try_tostring then
            local try = tostring(value)
            if try ~= nil then
                return try
            end
        end

        done[value] = true

        local list = {}
        for key in pairs(value) do list[#list + 1] = key end
        table.sort(list, function(a, b) return tostring(a) < tostring(b) end)
        local last = list[#list]

        local linefeed = (indent >= 0) and '\n' or ''
        local rep = "{" .. linefeed
        local comma
        for _, key in ipairs(list) do
            if key == last then
                comma = ''
            else
                comma = ','
            end
            local keyRep
            if type(key) == "number" then
                keyRep = key
            else
                keyRep = string.format("%q", tostring(key))
            end
            rep = rep ..
                      string.format("%s[%s] = %s%s%s",
                                    string.rep(" ", current_indent + indent),
                                    keyRep, table_dump_(value[key], indent,
                                                        current_indent + indent,
                                                        try_tostring, done),
                                                        comma, linefeed)
        end

        rep = rep .. string.rep(" ", current_indent) -- indent it
        rep = rep .. "}"

        done[value] = false
        return rep
    elseif type(value) == "string" then
        return string.format("%q", value)
    else
        return tostring(value)
    end
end

table.dump = function(value, indent, try_tostring)
    if value == nil then return 'nil' end
    indent = indent or 2
    local cid = 0
    local done = {}
    return table_dump_(value, indent, cid, try_tostring, done)
end
