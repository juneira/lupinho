table.merge = function(table1, table2)

    for index, value in ipairs(table2) do
        table.insert(table1, value)
    end

    for key, value in pairs(table2) do
        table1[key] = value
    end

    return table1
end
