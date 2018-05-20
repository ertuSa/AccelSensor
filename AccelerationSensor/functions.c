/*-------------------------------------------------------------------------------

						  Functions.c

                          This file contains function that are
						  used in main.c to make code more clearly
						  
						  
						  [16-05-2018] JackSparrow
						  Creation of this file.
						  
-------------------------------------------------------------------------------*/

#include "function.h"

/*******************************************************************************
*		Constants
********************************************************************************/



/*******************************************************************************
*		Variables
********************************************************************************/

static uint8_t I2C_Error = I2C_NoError;

/*******************************************************************************
*		Body of functions
********************************************************************************/

// Function that initialize I2C communication in uC.
void I2C_Init(void)
{
	TWCR = _BV(TWEA) | _BV(TWEN);
	I2C_SetBusSpeed(I2CBUSCLOCK / 100);
}

// Function that sets I2C bus speed in uC.
void I2C_SetBusSpeed(uint16_t speed)
{
	speed = (F_CPU / speed / 100 - 16) / 2;
	uint8_t prescaler = 0;
	while(speed > 255)
	{
		
		prescaler++;
		speed = speed / 4;
	}
	TWSR = (TWSR & (_BV(TWPS1) | _BV(TWPS0))) | prescaler;
	TWBR = speed;
}

// Function that generate START signal on I2C bus.
void I2C_Start(void)
{
	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
	I2C_WaitForComplete();
	if(TW_STATUS != TW_START) I2C_SetError(I2C_STARTError);
}

//Function that generate STOP signal on I2C bus.
void I2C_Stop(void)
{
	TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWSTO);
}

// Function that send address on I2C bus.
void I2C_SendAddr(uint8_t address)
{
	uint8_t status;
	if((address & 0x01) == 0)
	{
		status = TW_MT_SLA_ACK;
	}
	else
	{
		status = TW_MR_SLA_ACK;
	}
	TWDR = address;
	TWCR = _BV(TWINT) | _BV(TWEN);
	I2C_WaitForComplete();
	if(TW_STATUS != status)
	{
		I2C_SetError(I2C_NoNACK);
	}
}