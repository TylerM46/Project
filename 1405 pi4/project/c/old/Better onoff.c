#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define GPIO_PIN_5 5
#define GPIO_PIN_6 6

int main(void){
	int i;
	pinMode(GPIO_pin_5, OUTPUT);
	pinMode(GPIO_pin_6, OUTPUT);
	
	while(1){
		digitalWrite(GPIO_pin_5, HIGH);
		digitalWrite(GPIO_pin_6, HIGH);
		delayMicroseconds(100);
		
		digitalWrite(pin1, HIGH);
		digitalWrite(pin2, HIGH);
		delayMicroseconds(100);
		
	}
}
