require "src/lua/common"
require "src/lua/colors"
require "src/lua/log"

-- Binary handling libraries
require "src/lua/minecraft/packets"
require "src/lua/bin"

--[argv]: comes from C

settings = require "src/lua/argparse"

add_argument(
    "ip", {"ip", "-a"},
    "string", "127.0.0.2", "The IP of the server",
    "set", "ip"
)

add_argument(
    "port", {"port", "-p"},
    "number", "25565", "The PORT of the server",
    "set", "port"
)

parse(argv)




function basil_packet_manager(packet)
    print("Packet received!")

    for i, bin_num in pairs(packet) do
        local num = bin_to_dec(bin_num)

        local char = " "
        if num >= 32 and num <= 126 then char = string.char(num) end

        io.write(("%3d"):format(i))
        io.write(": [")
        write_color(("%4d %s"):format(num, char), colors.red)
        io.write("] ")

        for p = 8, 1, -1 do
            io.write(bin_num[p])

            --if p ~= 8 then io.write(" ") end
        end

        print()
    end

    os.exit()
end