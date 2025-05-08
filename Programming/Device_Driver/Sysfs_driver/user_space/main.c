#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *fp;
    int value, read_value;

    printf("Enter a value to write to /sys/kernel/my_sysfs/hello: ");
    scanf("%d", &value);

    // Write to sysfs file
    fp = fopen("/sys/kernel/my_sysfs/hello", "w");
    if (fp == NULL) {
        perror("Error opening sysfs file for writing");
        return EXIT_FAILURE;
    }
    fprintf(fp, "%d\n", value);
    fclose(fp);

    // Read back from sysfs file
    fp = fopen("/sys/kernel/my_sysfs/hello", "r");
    if (fp == NULL) {
        perror("Error opening sysfs file for reading");
        return EXIT_FAILURE;
    }
    if (fscanf(fp, "%d", &read_value) != 1) {
        perror("Failed to read back the value");
        fclose(fp);
        return EXIT_FAILURE;
    }
    fclose(fp);

    printf("Read back value: %d\n", read_value);

    return 0;
}

