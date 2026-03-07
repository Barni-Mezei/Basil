local lfs = require("lfs")

local logs = {}

-- Logs some data into a table
function log(log_type, log_text)
    logs[#logs + 1] = {
        time = os.time(),
        text = log_text,
        type = log_type,
    }
end

-- Outputs the log to the screen and logst it to the table
function print_log(log_type, log_text, log_color)
    local c = log_color or nil

    print_color(log_text, log_color)

    log(log_type, log_text)
end

-- Writes the logs out into a file
local function format_time(timestamp)
    return ("[%02d:%02d:%02d]"):format(
        (timestamp // 3600) % 24,
        (timestamp // 60) % 60,
        timestamp % 60
    )
end

function write_log(file_name)
    lfs.mkdir("logs")

    local file = io.open("logs/"..file_name, "w")
    for i, log in pairs(logs) do
        file:write(("%s[%s]: %s\n"):format(
            format_time(log.time),
            string.upper(log.type),
            log.text
        ))
    end
    file:close()
end