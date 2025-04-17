#include <sys/stat.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>

int main() {
struct stat file_stat;
if (stat("example.txt", &file_stat) == -1) {
    perror("Error getting file stats");
return 1;
}    
printf("File: example.txt\n");
printf("Size: %ld bytes\n", file_stat.st_size);
printf("Permissions: %o (octal)\n", file_stat.st_mode & 0777);
printf("Last modified: %s", ctime(&file_stat.st_mtime));
printf("Inode: %ld\n", file_stat.st_ino);
return 0;
}
