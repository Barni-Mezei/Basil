#include <stdio.h> 
#include <stdlib.h>
#include <string.h> 

// Networking libraries
#include <arpa/inet.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> // read(), write(), close()

// LUA libraries
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>



// Custom libraries
#include "colors.c"
#include "files.c"

#define SA struct sockaddr

// These values may be modified by the config file
char *IP = "127.0.0.1";
unsigned int PORT = 25565;

int main(int argc, char **argv) {
    print_color("Server started", color_green);

    //
    // LUA
    //

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
        if (i > 0) {
            lua_pushstring(L, argv[i]);  
            lua_seti(L, -2, i);
        }
    }
    lua_setglobal(L, "argv");  

    // Load main LUA scripts
    if (luaL_dofile(L, "src/lua/main.lua")) {
        const char *error_message = lua_tostring(L, -1);
        if (error_message != NULL) {
            printf("Error: %s\n", error_message);
        }
    }

    // TODO: load mods

    read_directories("mods");

    // TODO: load server config from server.properties

    // Parse

    lua_getglobal(L, "settings");
    lua_pushnil(L);  // First key
    while (lua_next(L, -2) != 0) {
        // Key is at -2, value is at -1
        if (lua_isstring(L, -2) && lua_isstring(L, -1)) {
            printf("%s = %s\n",
                lua_tostring(L, -2),
                lua_tostring(L, -1));
        }
        lua_pop(L, 1);  // Remove value, keep key
    }
    lua_pop(L, 1);  // Remove table

    lua_close(L);
    return 0;

    //
    // Networking
    //

    int sockfd, connfd, length;
    struct sockaddr_in servaddr, client;

    // socket create and verification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        print_color("Socket creation failed", color_red);
        lua_close(L);
        return 0;
    }

    print("Socket created successfully");

    bzero(&servaddr, sizeof(servaddr));

    // Assign IP and PORT 
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(IP);
    servaddr.sin_port = htons(PORT);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        print_color("Socket binding failed", color_red);
        lua_close(L);
        return 0;
    }

    print("Socket binding successful");

    // Listen on the created socket
    if ((listen(sockfd, 5)) != 0) {
        print_color("Listening failed", color_red);
        lua_close(L);
        return 0;
    }

    printf("You can connect on %s%s:%d%s\n", color_yellow, IP, PORT, color_white);

    length = sizeof(client);
  
    // Accept the data packet from the client
    connfd = accept(sockfd, (SA*)&client, (unsigned int*)&length);
    if (connfd < 0) {
        print_color("Server accept failed", color_red);
        lua_close(L);
        return 0;
    } 

    print("Client connected");

    // Function for chatting between client and server
    //  func(connfd);

    // Close the socket and the LUA environment
    close(sockfd);
    lua_close(L);

    print_color("Server stopped", color_red);
    return 0;
}