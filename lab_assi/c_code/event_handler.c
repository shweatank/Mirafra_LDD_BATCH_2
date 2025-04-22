#include<stdio.h>

enum event_name{
	EVENT_START=0,EVENT_PAUSE,EVENT_RESUME,EVENT_STOP
	};


void event_start(){
	printf("event start\n");
}
void event_pause(){
	printf("event pause\n");
}
void event_resume(){
	printf("event resume\n");
}
void event_stop(){
	printf("event stop\n");
}


void dispatcher(event_name E){
	if(E>=0 && E<4)
	(*ptr[E])();
	else
	printf("failed");
}

int main(){
	int (*ptr[4])()={&event_start,&event_pause,&event_resume,&event_stop};
	dispatcher(EVENT_START);
	dispatcher(EVENT_PAUSE);
	dispatcher(EVENT_RESUME);
	dispatcher(EVENT_STOP);

	return 0;

}
