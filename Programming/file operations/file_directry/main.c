#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>

int main() {
if (mkdir("my_folder", 0755) == -1) {
    perror("Error creating directory");
return 1;
}
    
printf("Directory created successfully.\n");
return 0;
}
