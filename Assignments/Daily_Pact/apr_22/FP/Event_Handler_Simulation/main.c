#include <stdio.h>

void onClick()   { puts("Button clicked."); }
void onHover()   { puts("Mouse hovering."); }
void onExit()    { puts("Mouse left.");     }

int main() {
    void (*events[])() = {onClick, onHover, onExit};

    for (int i = 0; i < 3; ++i)
        events[i]();

    return 0;
}

