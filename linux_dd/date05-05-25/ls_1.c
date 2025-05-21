#include <stdio.h>
#include <dirent.h>

int main(int argc, char *argv[]) {
    struct dirent *entry;
    DIR *dir;

    // If no directory is passed, use current directory
    const char *path = (argc > 1) ? argv[1] : ".";

    dir = opendir(path);
    if (dir == NULL) {
        perror("opendir");
        return 1;
    }

    while ((entry = readdir(dir)) != NULL) {
        // Skip . and ..
        if (entry->d_name[0] != '.') {
            printf("%s\n", entry->d_name);
        }
    }

    closedir(dir);
    return 0;
}
