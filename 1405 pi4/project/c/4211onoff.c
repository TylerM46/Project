#include <pigpio.h>
#include <stdio.h>

#define GPIO_pin_5 5
#define GPIO_pin_6 6

int main(void){
	if(gpioInitialise() <0){
	printf("failed :((");
	return 1;
	}
	
	gpioSetMode(GPIO_pin_5, PI_OUTPUT);
	gpioSetMode(GPIO_pin_6, PI_OUTPUT);
	
	while(1){
		gpioWrite(GPIO_pin_5, 1);
		gpioWrite(GPIO_pin_6, 1);
		gpioDelay(100);
		gpioWrite(GPIO_pin_5, 1);
		gpioWrite(GPIO_pin_6, 1);
		gpioDelay(100);
		gpioWrite(GPIO_pin_5, 1);
		gpioWrite(GPIO_pin_6, 1);
		gpioDelay(100);
		gpioWrite(GPIO_pin_5, 1);
		gpioWrite(GPIO_pin_6, 1);
		gpioDelay(100);
		
		gpioWrite(GPIO_pin_5, 0);
		gpioWrite(GPIO_pin_6, 0);
		gpioDelay(100);		
		gpioWrite(GPIO_pin_5, 0);
		gpioWrite(GPIO_pin_6, 0);
		gpioDelay(100);
		
		gpioWrite(GPIO_pin_5, 1);
		gpioWrite(GPIO_pin_6, 1);
		gpioDelay(100);
			
		gpioWrite(GPIO_pin_5, 0);
		gpioWrite(GPIO_pin_6, 0);
		gpioDelay(100);
		
	}
}
