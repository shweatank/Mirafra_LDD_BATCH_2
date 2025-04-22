#include <stdio.h>
#include <stdlib.h>

/* at a specific time Motor ON and OFF (8:00 to 10:00 */

#define POWER_ON  1
#define POWER_OFF 0

#define START_TIME 6
#define STOP_TIME 10

typedef struct Motor_funct
{
    void (*start)(int);
    void (*running)(int);
    void (*stop)(int);
}Motor;

void start(int time) {
    printf("POWER ON => Motor Started.\n\tTime Now: %d\n\n",time);
}
void run(int time) {
    printf("..........Motor is Running............\n\tTime Now: %d\n\n",time);
}
void stop(int time){
    printf("POWER OFF => Motor Stopped.\n\tTime Now: %d\n\n",time);
}

int motor_sw;
int ISR_check(int hour) {
    if(hour == START_TIME){
	motor_sw = POWER_ON;
    } 
    if(hour == STOP_TIME) 
	motor_sw = POWER_OFF;
}


int main()
{
    int time = 4;
    Motor motor = {start,run,stop};
    while(1)
    {
	if(time < START_TIME) printf("MOTOR not started.\n");
	else if(time == START_TIME) motor.start(time);
	else if(time < STOP_TIME) motor.running(time);
	else if(time == STOP_TIME) {
	    motor.stop(time);
	    break;
	}
	sleep(2);
	time++;
    }


	

    return 0;
}

