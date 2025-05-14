#include <wiringPi.h>
#include <stdio.h>

int main(void) {
	wiringPiSetupGpio();
	pinMode(5, PWM_OUTPUT);
	pwmSetMode(PWM_MODE_MS);
	pwmSetRange(1024);
	pwmSetClock(192); //19.2MHz/ 192 = 100kHz
	
	while(1){
		pwmWrite(18,512);
		delay(1000);
		}
		return(0);
	
	}
