#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <pthread.h>

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

#define SA struct sockaddr

typedef struct _basil_settings {
    long int ip;
    uint16_t port;
    char* modt;
    int max_players;
    int world_size;
    int world_type;
} basil_settings;

// Keep track of connected client IDs
#define MAX_CLIENTS 16
int clients[MAX_CLIENTS] = {};
int clientLen = 0;

#define MAX_PACKET_SIZE 16384
//#define MAX_PACKET_SIZE 2097152 // 2MB

// Include custom libraries
#include "lib.c"
#include "colors.c"
#include "files.c"

void* clientManager(void* sockfd) {
    struct sockaddr_in client;
    int length = sizeof(client);

    while (true) {
        // Accept the data packet from the client
        int connfd = accept(*(int*)sockfd, (SA*)&client, (unsigned int*)&length);

        printf("conn %d\n", connfd);

        if (connfd < 0) {
            print_color("Server accept failed", color_red);
        } 

        if (clientLen < MAX_CLIENTS) {
            clients[clientLen] = connfd;
            clientLen++;

            char str[32];
            snprintf(str, 32, "Client connected [%d/%d]", clientLen, MAX_CLIENTS);
            print_color(str, color_yellow);
        } else {
            print_color("Max clients reached!", color_red);
        }

        sleepms(1);
    }    
}

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

    // Load server.properties
    basil_settings settings = {};
    readServerConfig("server.properties", &settings);

    /*print_color("Exit", color_red);
    lua_close(L);
    return 0;*/ 

    //
    // Networking
    //

    int sockfd;
    struct sockaddr_in servaddr;

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
    servaddr.sin_addr.s_addr = settings.ip;
    servaddr.sin_port = ntohs(settings.port);

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

    // Print server logo
    printf("%s\n", basil_logo);

    // get back the IP in a string formt
    char strIp[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &settings.ip, strIp, INET_ADDRSTRLEN);

    printf("You can connect on %s%s:%d%s\n", color_yellow, strIp, settings.port, color_white);


    // Spawn client manager thread
    pthread_t clientThread;
    pthread_create(&clientThread, NULL, clientManager, &sockfd);

    // Listen for packaged from the connected clients
   
    char buff[MAX_PACKET_SIZE];
    int n;

    while (true) {
        printf("Client count: %d\n", clientLen);

        for (int clientIndex = 0; clientIndex < clientLen; clientIndex++) {
            printf("Emptying %d\n", clients[clientIndex]);
            bzero(buff, MAX_PACKET_SIZE);
    
            // read the message from client and copy it in buffer
            printf("Reading %d\n", clients[clientIndex]);
            int packetLength = read(clients[clientIndex], buff, sizeof(buff));

            // print buffer which contains the client contents
            printf("Buffer[%d]: %s\n", packetLength, buff);
            break;
        }

        sleepms(500);
    }

    pthread_join(clientThread, NULL);

    /*length = sizeof(client);

    // Accept the data packet from the client
    connfd = accept(sockfd, (SA*)&client, (unsigned int*)&length);
    if (connfd < 0) {
        print_color("Server accept failed", color_red);
        lua_close(L);
        return 0;
    } 

    print("Client connected");*/

    // Close the socket and the LUA environment
    close(sockfd);
    lua_close(L);

    print_color("Server stopped", color_red);
    return 0;
}