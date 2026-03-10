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