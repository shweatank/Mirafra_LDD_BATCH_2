#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

int main() {
if (unlink("data.txt") == -1) {
    perror("Error deleting file");
return 1;
}
if (open("data.txt", O_RDONLY) != -1) {
    printf("File still exists!\n");
    close(open("data.txt", O_RDONLY));
return 1;
}    
printf("File deleted successfully.\n");
return 0;
}
