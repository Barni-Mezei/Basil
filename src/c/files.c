#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dirent.h>

#define MAX_FILE_NAME_SIZE 257 // 256 + NULL terminator

int read_directories(const char* path) {
    DIR* directory = opendir(path);

    if (directory == NULL) {
        fprintf(stderr, "Can't open %s\n", path);
        return EXIT_FAILURE;
    }

    struct dirent* entry = NULL;
    while ((entry = readdir(directory)) != NULL) {
        char full_name[MAX_FILE_NAME_SIZE] = { 0 };
        snprintf(full_name, MAX_FILE_NAME_SIZE, "%s/%s", path, entry->d_name);

        if (entry->d_type == DT_DIR) {
            // Recurse unless the directory is the current (".") or parent ("..") directory.
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                printf("'%s' is a directory\n", full_name);
                read_directories(full_name);
            }
        } else {
            printf("'%s' is a file\n", full_name);
        }
    }

    closedir(directory);
    return EXIT_SUCCESS;
}

void trimString(char *str) {
    int start = 0;
    int end = strlen(str) - 1;

    // Remove trailing whitespace
    while (end > start && isspace(str[end])) {
        end--;
    }

    // If the string was trimmed, adjust the null terminator
    if (start > 0 || end < (strlen(str) - 1)) {
        memmove(str, str + start, end - start + 1);
        str[end - start + 1] = '\0';
    }
}

bool readServerConfig(const char* path, basil_settings* settings) {
    FILE* f;

    f = fopen(path, "r");
    
    if (f == NULL) return false;

    // Line buffer
    char lineBuf[128];

    inet_pton(AF_INET, "127.0.0.1", &settings->ip);
    settings->port = 25565;
    settings->modt = "Basil - A simple minecraft server";
    settings->max_players = 4;
    settings->world_size = 8;
    settings->world_type = 0;

    while(fgets(lineBuf, 128, f)) {
        trimString(lineBuf);
        char* propertyName = strtok(lineBuf, "=");
        char* propertyValue = strtok(NULL, "=");

        if (propertyValue != NULL) {
            if (strcmp(propertyName, "server-port") == 0) {
                settings->port = strtol(propertyValue, NULL, 10);
            }

            if (strcmp(propertyName, "modt") == 0) {
                settings->modt = propertyValue;
            }

            if (strcmp(propertyName, "max-players") == 0) {
                settings->max_players = strtol(propertyValue, NULL, 10);
            }

            if (strcmp(propertyName, "world-type") == 0) {
                if (strcmp(propertyValue, "stone")) settings->world_type = 1;
                if (strcmp(propertyValue, "noise")) settings->world_type = 2;
            }

            if (strcmp(propertyName, "world-size") == 0) {
                settings->world_size = strtol(propertyValue, NULL, 10);
            }

            //printf("'%s' = '%s'\n", propertyName, propertyValue);
        }
    }

    fclose(f);

    return true;
}