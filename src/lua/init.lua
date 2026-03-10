require "src/lua/common"
require "src/lua/colors"
require "src/lua/log"

settings = require "src/lua/argparse"

--[argv]: comes from C

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
