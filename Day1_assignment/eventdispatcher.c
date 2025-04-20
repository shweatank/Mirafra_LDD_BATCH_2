#include <stdio.h>

// Step 1: Define event codes
typedef enum {
    EVENT_START = 0,
    EVENT_STOP,
    EVENT_PAUSE,
    EVENT_RESUME,
    EVENT_COUNT  // Always keep this last to define array size
} EventCode;

// Step 2: Declare handler function signatures
void handleStart(void) {
    printf("Handling Start Event\n");
}

void handleStop(void) {
    printf("Handling Stop Event\n");
}

void handlePause(void) {
    printf("Handling Pause Event\n");
}

void handleResume(void) {
    printf("Handling Resume Event\n");
}

// Step 3: Create the dispatcher system
typedef void (*EventHandler)(void);  // Function pointer type

EventHandler dispatcher[EVENT_COUNT];  // Array of function pointers

// Step 4: Initialize the dispatcher
void initDispatcher() {
    dispatcher[EVENT_START]  = handleStart;
    dispatcher[EVENT_STOP]   = handleStop;
    dispatcher[EVENT_PAUSE]  = handlePause;
    dispatcher[EVENT_RESUME] = handleResume;
}

// Step 5: Function to dispatch an event
void dispatchEvent(EventCode event) {
    if (event >= 0 && event < EVENT_COUNT && dispatcher[event]) {
        dispatcher[event]();
    } else {
        printf("Invalid or unhandled event: %d\n", event);
    }
}

// Example usage
int main() {
    initDispatcher();

    dispatchEvent(EVENT_START);   // Output: Handling Start Event
    dispatchEvent(EVENT_PAUSE);   // Output: Handling Pause Event
    dispatchEvent(100);           // Output: Invalid or unhandled event: 100

    return 0;
}

