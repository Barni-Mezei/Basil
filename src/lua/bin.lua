-- Converts a table of bits into a number
-- Format: {LSB, 0/1, MSB}
function bin_to_dec(bits)
    local number = 0

    for i, v in ipairs(bits) do
        number = number + math.pow(2, i - 1) * v
    end

    return number
end

-- Converts a number into a binary table format
-- num: The number to encode
-- size: The size of the output table (number of bits)
function dec_to_bin(num, size)
    local s = size or 8
    local n = num

    local out = {}

    for i = s, 1, -1 do
        local result = n - math.pow(2, i - 1)

        if result > 0 then
            table.insert(out, 1)
            n = result
        else
            table.insert(out, 0)
        end
    end

    return out
end