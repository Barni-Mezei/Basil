require "src/common"
require "src/colors"
require "src/log"

local http = require("socket.http")
local ltn12 = require("ltn12")
local socket = require("socket")

settings = require "src/argparse"

-- Start program
local raw_args = { ... }

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

parse(raw_args)

-- Find the public IP of the server
print_log("info", "Starting...")

local response_body = {}
local res, code = http.request{
    url = "http://api.ipify.org",
    sink = ltn12.sink.table(response_body)
}

if code == 200 then
    settings.public_ip = table.concat(response_body)
else
    print_log("error", ("[HTTP Error]Failed to retrieve IP address: %s"):format(code), colors.red)
end

----------
-- MAIN --
----------

local tcp = socket.tcp()

function close_server()
    tcp:close()

    print_log("info", "Stopping server...", colors.red)
    write_log("latest.log")

    os.exit()
end

local res, err

-- Set target IP and PORT and
tcp:setoption("reuseaddr", true)
res, err = tcp:bind(settings.ip, settings.port)

if res == 1 then
    log("info", ("Server bound to: %s"):format(settings.ip))
    print("info", ("Server bound to: %s%s%s"):format(colors.yellow, settings.ip, colors.white))
else
    print_log("error", "[Binding error]: "..err, colors.red)
    close_server()
end

-- Start listening on the set ports (sets the maximum number of clients)
res, err = tcp:listen(16)

if res == 1 then
    log("info", ("Server is listening on: %d"):format(settings.port))
    print(("Server is listening on: %s%d%s"):format(colors.yellow, settings.port, colors.white))
else
    print_log("[Listening error]: "..err, colors.red)
    close_server()
end

print_color("ONLINE", colors.green)
print(("You can connect on: %s%s:%d%s"):format(colors.yellow, settings.public_ip, settings.port, colors.white))





-- Wait for a connection request
repeat
    res, err = tcp:accept()
until err == nil

print_log("info", "Someone connected!", colors.yellow)

local client = res

client:settimeout(500)
local data, err = client:receive()
pprint(data)





client:close()

close_server()

-- Set packet read timeout
--[[tcp:settimeout(1)

while true do
    local str = tcp:receive()

    if str then
        print(str)
    end
end]]