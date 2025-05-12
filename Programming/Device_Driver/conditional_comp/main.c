#include <stdio.h>

// Detect OS
#if defined(_WIN32) || defined(_WIN64)
    #define IS_WINDOWS 1
    #include <windows.h>
#else
    #define IS_WINDOWS 0
    #include <unistd.h>
#endif

// Detect architecture
#if defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__) || defined(__aarch64__)
    #define IS_64BIT 1
#else
    #define IS_64BIT 0
#endif

int main() {
    printf("Platform Info:\n");

    if (IS_WINDOWS)
        printf("Operating System: Windows\n");
    else
        printf("Operating System: Linux/Unix\n");

    if (IS_64BIT)
        printf("Architecture: 64-bit\n");
    else
        printf("Architecture: 32-bit\n");

    // OS-specific sleep
    printf("Sleeping for 1 second...\n");
#if IS_WINDOWS
    Sleep(1000); // milliseconds
#else
    sleep(1);    // seconds
#endif

    return 0;
}
