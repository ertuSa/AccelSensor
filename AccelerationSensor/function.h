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

#define	ACCEL_INIT_STATE	0
#define ACCEL_CONFIG_STATE	1
#define ACCEL_RUN_STATE		2
#define ACCEL_WORKING_STATE	4

#define ADR_INC_MASK		0x80

#define LIS3DH_R			0x31
#define LIS3DH_W			0x30

// LIS3DH registers
#define STATUS_REG_AUX		0x07
#define DA1					0
#define DA2					1
#define DA3					2
#define DA321				3
#define OR1					4
#define OR2					5
#define OR3					6
#define OR321				7

#define WHO_AM_I			0x0F

#define CTR_REG0			0x1E
#define SDO_PU_DISC			7

#define CTRL_REG1			0x20
#define XEN					0
#define YEN					1
#define ZEN					2
#define LPen				3
#define ODR0				4
#define ODR1				5
#define ODR2				6
#define ODR3				7

#define CTRL_REG2			0x21


#define CTRL_REG3			0x22


#define CTRL_REG4			0x23
#define SIM					0
#define ST0					1
#define ST1					2
#define HR					3
#define FS0					4
#define FS1					5
#define BLE					6
#define BDU					7

#define OUT_X_L				0x28
#define OUT_Y_L				0x2A
#define OUT_Z_L				0x2C

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
int8_t I2C_ReceiveDataByte_NACK(void);
int8_t I2C_ReceiveDataBytes_ACK(void);


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