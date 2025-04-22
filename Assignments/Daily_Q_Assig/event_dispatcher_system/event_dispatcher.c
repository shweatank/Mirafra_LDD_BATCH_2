#include <stdio.h>
#include <stdlib.h>

typedef enum {
    BUTTON_PRESS,
    SENSOR_TRIGGER,
    TIMEOUT,
    EVENT_COUNT
} EventType;

typedef void (*Handler)(void*);

typedef struct {
    Handler handlers[EVENT_COUNT];
} Dispatcher;

Dispatcher* setup_dispatcher();
void handle_event(Dispatcher* disp, EventType event, void* data);
void cleanup_dispatcher(Dispatcher* disp);
void on_button_press(void* data);
void on_sensor_trigger(void* data);
void on_timeout(void* data);

Dispatcher* setup_dispatcher() {
    Dispatcher* disp = malloc(sizeof(Dispatcher));
    if (!disp) {
        printf("Memory allocation failed!\n");
        exit(1);
    }

    disp->handlers[BUTTON_PRESS] = on_button_press;
    disp->handlers[SENSOR_TRIGGER] = on_sensor_trigger;
    disp->handlers[TIMEOUT] = on_timeout;

    return disp;
}

void handle_event(Dispatcher* disp, EventType event, void* data) {
    if (event < EVENT_COUNT && disp->handlers[event]) {
        disp->handlers[event](data);
    } else {
        printf("Bad event code: %d\n", event);
    }
}

void cleanup_dispatcher(Dispatcher* disp) {
free(disp);
}

void on_button_press(void* data) {
printf("Button pressed: %s\n", data ? (char*)data : "No data");
}

void on_sensor_trigger(void* data) {
printf("Sensor triggered: %s\n", data ? (char*)data : "No data");
}

void on_timeout(void* data) {
printf("Timeout occurred: %s\n", data ? (char*)data : "No data");
}

int main() {
Dispatcher* disp = setup_dispatcher();
char* data = "Some info";

handle_event(disp, BUTTON_PRESS, data);
handle_event(disp, SENSOR_TRIGGER, NULL);
handle_event(disp, TIMEOUT, data);
handle_event(disp, EVENT_COUNT, NULL); 
cleanup_dispatcher(disp);
return 0;
}