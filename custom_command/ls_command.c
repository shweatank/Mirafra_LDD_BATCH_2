#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

int main(int argc,char *argv[])
{
    DIR *dir;
    struct dirent *entry;
const char* path=(argc>1)?argv[1]:".";
    // Open the current directory
    dir = opendir(path);
    if (dir == NULL) {
        perror("opendir");
        return EXIT_FAILURE;
    }

    // Loop through directory entries
    while ((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);
    }

    // Close directory
    closedir(dir);
    return EXIT_SUCCESS;
}

