#include "stdio.h"
#include "stdlib.h"

// Include LUA
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

// Include other libraries
#include "src/c/colors.h"

#define IP "127.0.0.1"
#define PORT 25565

int main() {
    print_color("Server started!", color_green);

    lua_State *L = luaL_newstate();

    //luaL_checklstring(L, 1, NULL);

    //luaL_openlibs(L);

    // Execute Lua script
    if (luaL_dofile(L, "src/lua/test.lua")) {
        printf("Error: %s\n", lua_tostring(L, -1));
    }

    lua_close(L);

    print_color("Server stopped", color_red);
    return 0;
}