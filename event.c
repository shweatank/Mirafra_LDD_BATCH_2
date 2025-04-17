#include <stdio.h>
#include<stdlib.h>
void event_fun1()
{
    printf("Event1 is happend\n");
}
void event_fun2()
{
    printf("Event2 is happend\n");
}
typedef enum {
    EVENT1,
    EVENT2,
} Event;
int main()
{
    void (*ptr[2])()={&event_fun1,&event_fun2};
    while(1)
    {
        Event event=rand() % 2 == 0 ? EVENT1: EVENT2;
        ptr[event]();
    }
}
