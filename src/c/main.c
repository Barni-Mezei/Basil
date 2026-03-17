#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <signal.h>
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

bool mainIsRunning = false;
lua_State *L;

// Include custom libraries
#include "lib.c"
#include "colors.c"
#include "files.c"

//
// CODE
//
void exitHandler(int signalId) {
    mainIsRunning = false;
}

int luaExit(lua_State* L) {
    exitHandler(2);
    return 0;
}

void* connectionManager(void* sockfd) {
    struct sockaddr_in client;
    int length = sizeof(client);

    while (mainIsRunning) {
        // Accept the data packet from the client
        int connfd = accept(*(int*)sockfd, (SA*)&client, (unsigned int*)&length);

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

        sleepms(100);
    }
    
    return (void*)0;
}

void* packetManagerManager(void* sockfd) {
    char buff[MAX_PACKET_SIZE];

    while (mainIsRunning) {
        for (int clientIndex = 0; clientIndex < clientLen; clientIndex++) {
            // Empty out the buffer
            bzero(buff, MAX_PACKET_SIZE);
    
            // Read the buffer
            int packetLength = read(clients[clientIndex], buff, sizeof(buff));

            // Packet has data?
            if (packetLength != 0) {
                // Search for a packet handler LUA function
                lua_getglobal(L, "basil_packet_manager");

                // Is there a handler function available?
                if (lua_isfunction(L, -1)) {
                    // Construct table from the received numbers
                    lua_createtable(L, packetLength, 0);

                    for (int i = 0; i < packetLength; i++) {
                        // Add numbers in the following format:
                        /*
                        {1, 0, 0, 1, 0, 1, 1, 0, <- 8 bit repr. LSB -> MSB, 5 <- the signed number it self }
                        */
                        lua_createtable(L, 8, 0);

                        char n = buff[i]; 

                        // Add the number as raw bits
                        for (int j = 0; j < 8; j++) {
                            lua_pushinteger(L, (lua_Number)(n & 1));
                            lua_seti(L, -2, j + 1);

                            n = n >> 1;
                        }
                        // Add the raw number
                        //lua_pushinteger(L, (lua_Number)buff[i]);
                        //lua_seti(L, -2, 9);

                        lua_seti(L, -2, i + 1);
                    }

                    int status = lua_pcall(L, 1, 0, 0);
                    if (status != LUA_OK) {
                        const char *error_message = lua_tostring(L, -1);
                        if (error_message != NULL) {
                            printf("Error: %s\n", error_message);
                        }
                        lua_pop(L, 1);
                        exitHandler(2);
                    }
                } else {
                    print_color("No LUA packet handler found!", color_red);
                    exitHandler(2);
                }

            }

        }

        sleepms(100);
    }

    return (void*)0;
}

int main(int argc, char **argv) {
    print_color("Server started", color_green);

    //
    // LUA
    //

    L = luaL_newstate();

    // Load LUA standard libraries
    luaopen_base(L);
    luaopen_os(L);     lua_setglobal(L, "os");
    luaopen_string(L); lua_setglobal(L, "string");
    luaopen_io(L);     lua_setglobal(L, "io");
    luaopen_math(L);   lua_setglobal(L, "math");
    luaopen_table(L);  lua_setglobal(L, "os");
    luaopen_package(L);

    // Register exit handler
    lua_register(L, "c_exit", luaExit);

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

    // Set server IP if provided
    if (argc > 1) {
        inet_pton(AF_INET, argv[1], &settings.ip);
    }

    /*print_color("Exit", color_red);
    lua_close(L);
    return 0;*/ 

    //
    // Networking
    //

    int sockfd;
    struct sockaddr_in servaddr;

    struct linger sl;
    sl.l_onoff = 0;		/* non-zero value enables linger option in kernel */
    sl.l_linger = 0;	/* timeout interval in seconds */
    setsockopt(sockfd, SOL_SOCKET, SO_LINGER, &sl, sizeof(sl));

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

    //
    // Data polling and event loops
    //

    mainIsRunning = true;

    // Spawn a connection manager thread
    pthread_t connectionThread;
    pthread_create(&connectionThread, NULL, connectionManager, &sockfd);

    // Spawn a packet manager thread
    pthread_t packetManagerThread;
    pthread_create(&packetManagerThread, NULL, packetManagerManager, &sockfd);

    // Set event handling for Ctrl + C
    signal(SIGQUIT, exitHandler);
    signal(SIGINT, exitHandler);

    // Wait until a close event
    while (mainIsRunning) sleep(1);

    // Kill threads and wait for return
    pthread_cancel(connectionThread);
    pthread_cancel(packetManagerThread);
    pthread_join(connectionThread, NULL);
    pthread_join(packetManagerThread, NULL);

    // Close the socket and the LUA environment
    close(sockfd);
    lua_close(L);

    print_color("Server stopped", color_red);
    _exit(0);
}