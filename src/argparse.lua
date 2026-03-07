local settings = {}

local args = {}
local aliases = {}

-- arg_name ("username"): The name of the argument
-- arg_aliases ("username", "name", "-u"): The aliases for this argument
-- arg_type ("string"): The type of this argument (string|number|boolean|table)
-- arg_description ("The name of the user"): The description of this argument
-- arg_action ("set"): Action to do with the provided argument.
--            set: Modifies the setting at the provided path
--            flag: Sets the setting at the provided path to true if the artgument is presend
--            call: calls the provided function, with the input passed in as a string
-- arg_path ("user.name"): The path of the setting inthe settings table or a callable
function add_argument(arg_name, arg_aliases, arg_type, arg_default, arg_description, arg_action, arg_path)
    args[#args + 1] = {
        alias = arg_aliases,
        name = arg_name,
        type = arg_type,
        default = arg_default,
        description = arg_description,

        action = arg_action,
        path = arg_path,
    }
end

-- Converts [str] into the given type. Valid types are: string|number|boolean
local function convert(str, target_type)
    if target_type == "string" then return str end
    if target_type == "number" then return tonumber(str) end
    if target_type == "boolean" then return str == "1" or str == "true" end

    return str
end


local function generate_aliases()
    for arg_index,arg in pairs(args) do
        for _,alias in pairs(arg.alias) do
            aliases[alias] = {
                index = arg_index,
                action = arg.action,
            }

            settings[arg.path] = convert(arg.default, arg.type)
        end
    end
end

-- Prints a colored help message line
function print_help(arg_name, arg_type, default_value, description)
    io.write(arg_name.." ")
    write_color("("..arg_type..")", colors.gray)
    io.write(": ")
    write_color(default_value, colors.yellow)
    print(" - "..description)
end

function list_arguments()
    for _,arg in pairs(args) do
        print_help(arg.name, arg.type, arg.default, arg.description)
    end
end

local function set_setting(path, arg_name, arg_value, arg_type)
    settings[path] = convert(arg_value, arg_type)
    print_color(("Set %s: %s"):format(
        string.upper(arg_name),
        tostring(arg_value)
    ), colors.gray)
end

-- Parsesd the raw command line argumenmts and fill s up the settings table
function parse(raw_args)
    generate_aliases()

    local arg_index = nil

    for i = 1, #raw_args do
        -- Display help message
        if raw_args[i] == "help" or raw_args[i] == "-h" then
            list_arguments()

            os.exit()
        end

        -- Execute argument action
        if arg_index then
            local action = args[arg_index].action
            local arg_type = args[arg_index].type
            local path = args[arg_index].path
            local name = args[arg_index].name

            if action == "set" then
                set_setting(path, name, raw_args[i], arg_type)
            end

            -- Call the specified function
            if action == "call" then
                path(raw_args[i])
            end

            arg_index = nil
        end

        -- Get argument type
        if arg_index == nil and aliases[raw_args[i]] ~= nil then
            local alias_data = aliases[raw_args[i]]
            if alias_data.action == "flag" then
                local arg_data = args[alias_data.index]
                set_setting(arg_data.path, arg_data.name, "1", "boolean")
            else
                arg_index = alias_data.index
            end
        end 
    end
end

return settings