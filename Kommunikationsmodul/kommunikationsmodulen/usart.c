﻿/*
 * CFile1.c
 *
 * Created: 4/7/2014 11:34:04 AM
 *  Author: samli177
 */ 

// -- USART Stuff --



#include <avr/io.h>
#include "fifo.h"
#include "usart.h"


void USART_init()
{
	//set baud rate
	//typecasting of "int" to byte truncates to the lowest uint8_t
	UBRR0H = (uint8_t) (((F_CPU / 16 / BaudRate ) - 1)>>8);
	UBRR0L = (uint8_t) ((F_CPU / 16 / BaudRate ) - 1) ;

	//enable receiver and transmitter and enable receive interrupt
	UCSR0B = (1<<RXEN0)|(1<<TXEN0|(1<<RXCIE0));

	//Frame format: 8data, no parity, 1 stop bit
	UCSR0C = (1<<UCSZ00 | 1<<UCSZ01);

}

uint8_t USART_CheckRxComplete()
{
	return (UCSR0A & (1<<RXC0)); // zero if no data is available to read
}

uint8_t USART_CheckTxReady()
{
	return (UCSR0A & (1<<UDRE0)); // zero if transmit register is not ready to receive new data
}

void USART_WriteByte(uint8_t DataByteOut)
{
	while(USART_CheckTxReady() == 0)
	{;;} // wait until ready to transmit NOTE: Can probably be optimized using interrupts
	UDR0 = DataByteOut;
}

uint8_t USART_ReadByte()
{
	// NOTE: check if data is available before calling this function. Should probably be implemented w. interrupt.
	return UDR0;

}


// calculate crc for packet
uint16_t USART_crc16(uint8_t tag, uint8_t length)
{
	int count;
	uint16_t data, i;
	unsigned int crc; // maybe use uint16_t here
	
	crc = 0xffff;
	
	if (length == 0)
	return (~crc);
	
	for(count = -2; count < length ; ++count)
	{
		if(count == -2)
		{
			data = tag;
		}else if(count == -1)
		{
			data = length;
		}else
		{
			data = gTxPayload[count];
		}
		
		for (i = 0; i < 8; ++i)
		{
			if ((crc & 0x0001) ^ (data & 0x0001))
			{
				crc = (crc >> 1) ^ POLY;
			}else
			{
				crc >>= 1;
			}
			
			data >>= 1;
		}
	}
	
	crc = ~crc;
	
	data = crc;
	crc = (crc << 8 | (data >> 8 & 0xff));
	
	return (crc);
}

void USART_SendPacket(char tag, uint8_t length)
{
	int count, offset, buffersize;
	uint16_t crc;
	
	// put start of packet into transmit buffer
	gTxBuffer[0] = 0x7e; // frame delimiter
	gTxBuffer[1] = tag;
	gTxBuffer[2] = length;
	
	// payload
	
	offset = 3;
	
	for(count = 0; count < length; ++count)
	{
		if(gTxPayload[count] == 0x7e || gTxPayload[count] == 0x7d) // if frame delimiter or escape octet occurs in data
		{
			gTxBuffer[count + offset] = 0x7d; // add the escape octet
			offset++;
			gTxBuffer[count + offset] = (1<<5)^gTxPayload[count]; // invert bit 5 in data
		}else
		{
			gTxBuffer[count + offset] = gTxPayload[count];
		}
	}
	
	crc = USART_crc16(tag, length);
	
	gTxBuffer[count+offset] = (uint8_t)(crc >> 8);
	++count;
	gTxBuffer[count + offset] = (uint8_t)(crc);
	++count;
	
	
	gTxBuffer[count+offset] = 0x7e;
	
	buffersize = count + offset;
	
	for(count = 0; count < buffersize + 1; ++count)
	{
		USART_WriteByte(gTxBuffer[count]);
	}
}

void USART_SendMessage(char msg[])
{
	for(int i = 0; i < strlen(msg); ++i )
	{
		gTxPayload[i] = msg[i];
	}
	
	USART_SendPacket('M', strlen(msg));
}


uint8_t USART_DecodeMessageRxFIFO()
{
	
	uint8_t *len = 0;
	uint8_t *character = 0;
	
	if(FifoRead(gRxFIFO, len))
	{
		send_string(S_ADRESS, "RxFIFO ERROR: LEN MISSING");
		return 1; // error
	}
	
	int length = *len; // I don't know why I can't use *len directly... but it took me 4h to figure out that you can't do it....
	
	//NOTE: there has to be a better way of doing this...
	int ifzero = 0;
	if(length == 0) ifzero = 1;
	uint8_t msg[length-1+ifzero];

	for(int i = 0; i < length; ++i)
	{
		if(FifoRead(gRxFIFO, character))
		{
			send_string(S_ADRESS, "RxFIFO ERROR: DATA MISSING");
			return 1; // error
		}

		msg[i] = *character;
	}
	
	
	// TODO: send to relevant party... the display for now
	send_string_fixed_length(S_ADRESS, msg, length);
	return 0;
}

void USART_DecodeRxFIFO()
{
	uint8_t *tag = 0;
	
	if(!(FifoRead(gRxFIFO, tag))) // if the buffer is NOT empty
	{
		switch(*tag){
			case('M'): // if 'tag' is 'M'
			{
				if(USART_DecodeMessageRxFIFO()) // if decoding failed
				{
					// TODO: flush buffer?
					return;
				}
				
				break;
			}
		}
	}
}

void USART_Bounce()
{
	for(int i = 0; i < gRxBuffer[1]; i++)
	{
		gTxPayload[i] = gRxBuffer[i+2];
	}
	USART_SendPacket(gRxBuffer[0], gRxBuffer[1]);
}


// -- END USART stuff --