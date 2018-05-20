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

#define F_CPU			1000000

#define I2C_NoError		0
#define I2C_STARTError	1
#define I2C_NoNACK		3
#define I2C_NoACK		4

/*******************************************************************************
*		Macros
********************************************************************************/

#define _BV(bit) (1 << (bit))

/*******************************************************************************
*		Constants
********************************************************************************/

const uint16_t I2CBUSCLOCK = 100;

/*******************************************************************************
*		Variables
********************************************************************************/

static uint8_t I2C_Error;

/*******************************************************************************
*		Functions
********************************************************************************/

void I2C_Init(void);
void I2C_SetBusSpeed(uint16_t speed);
void I2C_Start(void);
void I2C_SendAddr(uint8_t address);


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

#endif