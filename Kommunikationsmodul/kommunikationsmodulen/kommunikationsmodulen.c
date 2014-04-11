/*
 * kommunikationsmodulen.c
 *
 * Created: 4/3/2014 10:12:45 AM
 *  Author: samli177
 */ 


#define F_CPU 18432000
#define BaudRate 115200

#define POLY 0x8408

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "twi.h"
#include "fifo.h"
#include "usart.h"


// -- Declarations --
void init();



// -- MAIN --

int main(void)
{
	init();
	USART_init();
	
	// init TWI
	TWI_init(C_ADRESS);
	
	sei();
	_delay_ms(500);
	while(1)
	{
		PORTA ^= (1<<PORTA0);
		
		
		USART_DecodeRxFIFO();
		USART_SendSensors();
		
		_delay_ms(1000);
		if(TWI_send_status(S_ADRESS))
			PORTA ^= (1<<PORTA1);
	}
}

// --  END MAIN --


void init()
{
	DDRA |= (1<<PORTA0|1<<PORTA1); //set status diodes to outputs
	
}



// -- Interrupts -- 
