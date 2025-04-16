#include <stdio.h>
enum eventcode{
    EVENT_1,
    EVENT_2,
    EVENT_3
};
void EVENT1(){
    printf("event1 occurred\n");
}
void EVENT2(){
    printf("event2 occurred\n");
}
void EVENT3(){
    printf("event3 occurred\n");
}
int main() {
    int a=0;
    void (*ptr[4])(void)={EVENT1,EVENT2,EVENT3};
    printf("enter 0 to 2 randomly to assume events are occured externally\n");
    while(1){
    scanf("%d",&a);
    switch(a){
    case EVENT_1:
                ptr[EVENT_1]();
                break;
    case EVENT_2:
                ptr[EVENT_2]();
                break;
    case EVENT_3:
                ptr[EVENT_3]();
                break;
    default :
               printf("invalid event occured\n");
    }
    
    }
    return 0;
}
