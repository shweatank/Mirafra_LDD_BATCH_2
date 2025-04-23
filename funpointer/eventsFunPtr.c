#include <stdio.h>


enum eventcodes{
	event1=1,event2,event3,EVENTCODES
};

void event1_code(void){
	printf("event1code\n");
}
void event2_code(void){
	printf("event2code\n");
}
void event3_code(void){
	printf("event3code\n");
}
void (*func[4])(void) = {NULL,event1_code,event2_code,event3_code};

void event_dispatcher(int event_code){
	if(event_code >= 1 && event_code < EVENTCODES){
		(*func[event_code])();
	} else{
		printf("entered wrong code\n");
	}
}
int main()
{
	for(int i = 0; i < EVENTCODES;i++) {
		event_dispatcher(i);
	}

	return 0;
}
