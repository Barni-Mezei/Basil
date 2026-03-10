#include "stdio.h"
#include "stdlib.h"

// Include LUA
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

// Include other libraries
#include "colors.h"

#define IP "127.0.0.1"
#define PORT 25565

int main(int argc, char **argv) {
    print_color("Server started", color_green);

    lua_State *L = luaL_newstate();

    // Load LUA standard libraries
    luaopen_base(L);
    luaopen_os(L);     lua_setglobal(L, "os");
    luaopen_string(L); lua_setglobal(L, "string");
    luaopen_io(L);     lua_setglobal(L, "io");
    luaopen_math(L);   lua_setglobal(L, "math");
    luaopen_table(L);  lua_setglobal(L, "os");
    luaopen_package(L);

    // Give command line arguments to the LUA env
    lua_createtable(L, argc, 0);
    for (int i = 0; i < argc; i++) {
        lua_pushstring(L, argv[i]);  
        lua_seti(L, -2, i + 1);
    }
    lua_setglobal(L, "argv");  


    // Execute Lua script
    if (luaL_dofile(L, "src/lua/init.lua")) {
        const char *error_message = lua_tostring(L, -1);
        if (error_message != NULL) {
            printf("Error: %s\n", error_message);
        }
    }

    lua_close(L);

    print_color("Server stopped", color_red);
    return 0;
}