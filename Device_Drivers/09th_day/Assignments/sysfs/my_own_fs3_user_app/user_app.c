#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *fp;
    int value;

    printf("Enter a value to write to /sys/kernel/my_sysfs/hello: ");
    scanf("%d", &value);

    fp = fopen("/sys/kernel/my_sysfs/hello", "w");
    if (fp == NULL) {
        perror("Error opening sysfs file");
        return EXIT_FAILURE;
    }

    fprintf(fp, "%d\n", value);
    fclose(fp);

    printf("Value %d written to /sys/kernel/my_sysfs/hello\n", value);
    return 0;
}
