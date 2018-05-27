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
	I2C_SetBusSpeed();
	TWCR = _BV(TWEA) | _BV(TWEN);
}

// Function that sets I2C bus speed in uC.
void I2C_SetBusSpeed(uint16_t speed)
{
	TWSR |= 0x00;	// TWSR = 
	TWBR = _BV(TWBR6) | _BV(TWBR3);		// TWBR = 0x48
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
void I2C_SendDevAddr(uint8_t address)
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

// Function that send start signal and device address that we want to communicate on I2C bus.
void I2C_SendStartAndSelect(uint8_t addr)
{
	I2C_Start();
	I2C_SendDevAddr(addr);
}

// Function that send data to device through I2C bus.
void I2C_SendByte(uint8_t byte)
{
	TWDR = byte;
	TWCR = _BV(TWINT) | _BV(TWEN);
	I2C_WaitForComplete();
	if(TW_STATUS != TW_MT_DATA_ACK)
	{
		I2C_SetError(I2C_NoACK);
	}
}

// Function that receive data byte from device through I2C bus.
int8_t I2C_ReceiveDataByte_NACK(void)
{
	TWCR = _BV(TWINT) | _BV(TWEN);
	I2C_WaitForComplete();
	if(TW_STATUS != TW_MR_DATA_NACK)
	{
		I2C_SetError(I2C_NoNACK);
	}
	return TWDR;
}

// Function that receive data bytes(more then one byte) from device through I2C bus.
int8_t I2C_ReceiveDataBytes_ACK(void)
{
	TWCR = _BV(TWEA) | _BV(TWINT) | _BV(TWEN);
	I2C_WaitForComplete();
	if(TW_STATUS != TW_MR_DATA_ACK)
	{
		I2C_SetError(I2C_NoACK);
	}
	return TWDR;
}