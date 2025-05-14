#include <pigpio.h>
#include <stdio.h>

#define GPIO_pin_5 5
#define GPIO_pin_6 6

int main(void){
	if(gpioInitialise() <0){
	printf("failed :((");
	return 1;
	}
	int data1[20] ={0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1};
	int data2[20] ={0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1};
	
	gpioSetMode(GPIO_pin_5, PI_OUTPUT);
	gpioSetMode(GPIO_pin_6, PI_OUTPUT);
	
	while(1){
		for (int i=0; i<20; i++){ 
			gpioWrite(GPIO_pin_5, data2[i]);
			gpioWrite(GPIO_pin_6, data1[i]);
			gpioDelay(10);
		}
	}
}
