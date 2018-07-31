#ifndef _FUNCTIONS_H
#define _FUNCTIONS_H

/*-------------------------------------------------------------------------------

						  Functions.h

                          Header file for functions.c
						  
						  
						  [16-05-2018] JackSparrow
						  Creation of this file.
						  
-------------------------------------------------------------------------------*/

#include <util/twi.h>

/*******************************************************************************
*		Defines
********************************************************************************/

#define I2C_NoError		0
#define I2C_STARTError	1
#define I2C_NoNACK		3
#define I2C_NoACK		4

#define ADR_INC_MASK		0x80

#define TRUE	1U
#define FALSE	0U





/*******************************************************************************
*		Macros
********************************************************************************/

#define _BV(bit) (1 << (bit))

/*******************************************************************************
*		Constants
********************************************************************************/



/*******************************************************************************
*		Variables
********************************************************************************/

enum accelState
{
	ACCEL_INIT_MM,		
	ACCEL_CONFIG_STATE,	
	ACCEL_INIT_DATA,
	ACCEL_RUN_STATE,		
	ACCEL_WORKING_STATE,	
	ACCEL_COMP_STATE,		
};

typedef struct
{
	int16_t x;
	int16_t y;
	int16_t z;
}	accels_ts;

typedef struct
{
	long x;
	long y;
	long z;
}	accels_ts_i32;

static uint8_t I2C_Error;

/*******************************************************************************
*		Functions
********************************************************************************/

void I2C_Init(void);
void I2C_SetBusSpeed();
void I2C_Start(void);
void I2C_Stop(void);
void I2C_SendDevAddr(uint8_t address);
void I2C_SendStartAndSelect(uint8_t addr);
void I2C_SendByte(uint8_t byte);
uint8_t I2C_ReceiveDataByte_NACK(void);
uint8_t I2C_ReceiveDataBytes_ACK(void);


static inline void I2C_WaitForComplete(void)
{
	while(!(TWCR & _BV(TWINT)))
	{
		// Waiting until interrupt flag is set by hardware when TWI has finished its current job.
	}
}

static inline void I2C_SetError(uint8_t err)
{
	I2C_Error = err;
}
/*
static inline void USART_Init(unsigned int ubrr)
{
	// Set baud rate
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	// Enable receiver and transmitter
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);
	// Set frame format: 8 data bit, 1 stop bit 
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
}*/
/*
static inline void USART_Transmit(char data)
{
	
	// Wait for empty transmit buffer 
	while( !(UCSR0A & _BV(5)) )
	{
		;
	}
	UDR0 = data;
}*/
/*
static inline void USART_Transmit_I16(int8_t data)
{
	
	// Wait for empty transmit buffer 
	while( !(UCSR0A & _BV(5)) )
	{
		;
	}
	UDR0 = data;
}
*/


#endif