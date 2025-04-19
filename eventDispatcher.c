/*
1. Event Dispatcher System
Problem Statement:
Implement an event dispatcher that maps integer event codes to different handler functions using function pointers. When a particular event code is received, the dispatcher should call the corresponding handler function.

Skills Used: Array of function pointers, mapping enums to functions.
*/

#include<stdio.h>

typedef enum{
start =0,
stop,
pause,
max
}code;

void event_start()
{
printf("EVENT STARTED\n");
}

void event_stoped()
{
printf("EVENT STOPED\n");
}

void event_pause()
{
printf("EVENT PAUSED\n");
}

void (*event_ptr[max])()={
 &event_start,
 &event_stoped,
 &event_pause
};


void event_dispatcher(code Code)
{
	if((Code >= 0) &&(Code < max))
	{
		(*event_ptr[Code])();
	}

}
int main()
{
	event_dispatcher(start);
	event_dispatcher(stop);
	event_dispatcher(pause);
	

	
	return 0;
}


