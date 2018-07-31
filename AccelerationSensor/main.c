/*
 *	AccelerationSensor.c
 *
 *	Created: 16.05.2018 23:01:13
 *	Author : wyzku
 *
 *	Notes:
 *		-	define ASIC type(LIS3DH/MPU_9150) in Registers header file
 */ 
#include "MPU_6050_REG.h"		

#define F_CPU				16000000UL
#define MYUBRR				0x67 /* decimal 103 = baud rate 9600 */

#define ACCEL_TRESHOLD		31
#define ACCEL_CONF_BITMASK	0x03
#define GYRO_CONF_BITMASK	0x0C

#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include <stdlib.h>
#include "function.h"

/**********************************************
*	Constants
***********************************************/

const double PI = 3.141592;
const long COUNT_NO = 100;

/**********************************************
*	Variables
***********************************************/

short int readBuffer;
short int state;
short int n;
short int accelConf;
short int gyroConf;

unsigned int tempX;
unsigned int tempY;
unsigned int tempZ;

int count;

long x;
long y;
long z;
long xInit;
long yInit;
long zInit;

_Bool zAlarm;

double angleX;
double angleY;
double angleX_Init;
double angleY_Init;

char bufforX[11];
char bufforY[11];
char bufforZ[11];
char bufforX_Init[11];
char bufforZ_Init[11];
char bufforAngleX[10];
char bufforAngleY[10];

accels_ts accelsTemp;

accels_ts_i32 accels;

/**********************************************
*	Source code
***********************************************/

int main(void)
{
	state = ACCEL_INIT_MM;
	zAlarm = FALSE;
	
    while (1) 
    {
		
		switch(state)
		{
		
			
		/* State that initiate I/O and communication ports of uC */
		case ACCEL_INIT_MM:
			/* configuration of uC, I/O pins */
			DDRB |= _BV(PINB2);		/* configured as an output */
			DDRC &= ~(_BV(PINC0) | _BV(PINC1) | _BV(PINC2) | _BV(PINC3)); /* configured as an input */
			PORTC |= 0x0F; /* input pull-up configuration */
			PORTB |= _BV(PINB0);
			
			/* TODO: ??? */
			//PRR &= ~( _BV(PRTWI) | _BV(PRUSART0) );
			
			/* Initialization of USART communication */
			//USART_Init(MYUBRR);
			/* Initialization of I2C communication */
			I2C_Init();
			
			/* Reading configuration for accel and gyro full scale range */
			accelConf = PINC & ACCEL_CONF_BITMASK;
			gyroConf = PINC & GYRO_CONF_BITMASK;
			
			/* Changing the state of state machine */
			state = ACCEL_CONFIG_STATE;
			break; /* End of ACCEL_INIT_MM */
		
		
		/* State that configure slave device */	
		case ACCEL_CONFIG_STATE:
			
			I2C_SendStartAndSelect(MPU_6050_W);
			I2C_SendByte(SMPRT_DIV_REG);
			/* Setting up sample rate of MPU-6050 (Gyro Fs(1kHz)/(1 + SMPRT_DIV_RE(0x04)) = 200Hz */
			I2C_SendByte(0x04);
			/* Setting up Gyro Fs 0x01 = 1kHz */	
			I2C_SendByte(0x01);
			/* Setting up gyro full scale range(0 - 250deg/s, 1 - 500deg/s, 2 - 1000deg/s, 3 - 2000deg/s) */		
			I2C_SendByte(gyroConf);
			/* Setting up accel full scale range(0 - 2g, 1 - 4g, 2 - 8g, 3 - 16g); */
			I2C_SendByte(accelConf);
			I2C_Stop();
			
			
			I2C_SendStartAndSelect(MPU_6050_W);
			I2C_SendByte(INT_ENABLE_REG);
			/* Data ready interrupt enabled */
			I2C_SendByte(0x01);
			I2C_Stop();
			
			
			I2C_SendStartAndSelect(MPU_6050_W);
			I2C_SendByte(PWR_MGMT_1_REG);
			/* Temperature sensor disabled and clock set to X axis gyro reference */
			I2C_SendByte(0x09);
			I2C_Stop();
			
			
			/* Checking the communication with device */
			I2C_SendStartAndSelect(MPU_6050_W);
			I2C_SendByte(WHO_AM_I_REG);
			I2C_SendStartAndSelect(MPU_6050_R);
			readBuffer = I2C_ReceiveDataByte_NACK();	
			I2C_Stop();

			//USART_Transmit(readBuffer);
			
			if(0x68 == (readBuffer & ~(0x81)))
			{
				/* If communication with device is working ***********/
				for(n = 0; n < 20; n++)								 //
				{													 //
					PORTB ^= _BV(PINB5);							 //
					_delay_ms(100);									 //
				}													 //
				/*****************************************************/
				state = ACCEL_INIT_DATA;	
			}
			else
			{
				// TODO: Need to add code that reset uC if communication is wrong
				/* If there is fault in communication with device ****/
				PORTB |= _BV(PB5);									 //
				for(n = 0; n < 10; n++)								 //
				{													 //
					PORTB ^= _BV(PINB5);							 //
					_delay_ms(500);									 //					
				}													 //
				/*****************************************************/
			}
			break; /* End of ACCEL_CONFIG_STATE case */
			
	
		case ACCEL_INIT_DATA:
			I2C_SendStartAndSelect(MPU_6050_W);
			I2C_SendByte(INT_STATUS_REG);
			I2C_SendStartAndSelect(MPU_6050_R);
			readBuffer = I2C_ReceiveDataByte_NACK();
			I2C_Stop();
			/*	Average values for each axis from 100 samples from device is used 
			*	to calculate reference value of slope angle */
			if(_BV(DATA_RDY_INT) & readBuffer)
			{
				I2C_SendStartAndSelect(MPU_6050_W);
				I2C_SendByte(ACCEL_XOUT_H_REG);
				I2C_SendStartAndSelect(MPU_6050_R);
				/* Taking reading from MPU-6050 */
				accelsTemp.x = (I2C_ReceiveDataBytes_ACK() << 8);
				accelsTemp.x |= I2C_ReceiveDataBytes_ACK();
				accelsTemp.y = (I2C_ReceiveDataBytes_ACK() << 8);
				accelsTemp.y |= I2C_ReceiveDataBytes_ACK();
				accelsTemp.z = (I2C_ReceiveDataBytes_ACK() << 8);
				accelsTemp.z |= I2C_ReceiveDataByte_NACK();
				I2C_Stop();
				
				accels.x += (long)accelsTemp.x;
				accels.y += (long)accelsTemp.y;
				accels.z += (long)accelsTemp.z;
				count++;
				accelsTemp.x = 0;
				accelsTemp.y = 0;
				accelsTemp.z = 0;
			
				if(COUNT_NO == count)
				{
					/* Calculation of average acceleration value from 100 readings */
					xInit = (long)(accels.x/COUNT_NO);
					yInit = (long)(accels.y/COUNT_NO);
					zInit = (long)(accels.z/COUNT_NO);
					
					/* Slop angle calculation using arctg function */
					angleX_Init = (atan2((double)(xInit), (double)(zInit)) / (PI/180));
					angleY_Init = (atan2((double)(yInit), (double)(zInit)) / (PI/180));
					
					/* Reseting variables that be used later */
					count = 0;
					accels.x = 0;
					accels.y = 0;
					accels.z = 0;
				
					state = ACCEL_RUN_STATE;
					
					/* TODO: when code is finish need to be deleted */
					//ltoa(xInit, bufforX_Init, 10);
					//ltoa(zInit, bufforZ_Init, 10);
				}
			}
			break; /* End of ACCEL_INIT_STATE case */
			
			
		/* State that control get acceleration values from slave device */
		case ACCEL_RUN_STATE:
			I2C_SendStartAndSelect(MPU_6050_W);
			I2C_SendByte(INT_STATUS_REG);
			I2C_SendStartAndSelect(MPU_6050_R);
			readBuffer = I2C_ReceiveDataByte_NACK();
			I2C_Stop();
			if(_BV(DATA_RDY_INT) & readBuffer)
			{
				I2C_SendStartAndSelect(MPU_6050_W);
				I2C_SendByte(ACCEL_XOUT_H_REG);
				I2C_SendStartAndSelect(MPU_6050_R);
				accelsTemp.x = (I2C_ReceiveDataBytes_ACK() << 8);
				accelsTemp.x |= I2C_ReceiveDataBytes_ACK();
				accelsTemp.y = (I2C_ReceiveDataBytes_ACK() << 8);
				accelsTemp.y |= I2C_ReceiveDataBytes_ACK();
				accelsTemp.z = (I2C_ReceiveDataBytes_ACK() << 8);
				accelsTemp.z |= I2C_ReceiveDataByte_NACK();
				I2C_Stop();
				
				state = ACCEL_WORKING_STATE;
				/* TODO: Need to adjust condition, maybe it need to be moved somewhere else */
				/*
				if(1000 < abs((zInit < accelsTemp.z) ? (accelsTemp.z - zInit) : (zInit - accelsTemp.z)))
				{
					PORTB |= _BV(PINB5);
					_delay_ms(1000);
					zAlarm = TRUE;
				}
				else
				{
					if(zAlarm) 
					{
						PORTB &= ~(_BV(PINB5));
						zAlarm = FALSE;
					}
				}
				*/
			}
			break; /* End of ACCEL_RUN_STATE case */
			
			
		/* State that compare acceleration values from memory and values get from acceleration sensor */	
		case ACCEL_WORKING_STATE:
			state = ACCEL_RUN_STATE;
			accels.x += (long)accelsTemp.x;
			accels.y += (long)accelsTemp.y;
			accels.z += (long)accelsTemp.z;
			count++;
			accelsTemp.x = 0;
			accelsTemp.y = 0;
			accelsTemp.z = 0;
			
			if(COUNT_NO == count)
			{
				
				x = (long)(accels.x/COUNT_NO);
				y = (long)(accels.y/COUNT_NO);
				z = (long)(accels.z/COUNT_NO);
				count = 0;
				accels.x = 0;
				accels.y = 0;
				accels.z = 0;
				
				angleX = -(atan2((double)(x), (double)(z))/ (PI/180));
				angleY = -(atan2((double)(y), (double)(z)) / (PI/180));
				
				state = ACCEL_COMP_STATE;
			}	
			break; /* End of ACCEL_WORKING_STATE case */
		
		
		case ACCEL_COMP_STATE:
			/* TODO: Need to adjust it and add option to chose on what condition alarm should be turned on */
			if(10 < abs(angleX > angleX_Init ? angleX - angleX_Init : angleX_Init - angleX))
			{
				PORTB |= _BV(PINB2);
			}
			else
			{
				PORTB &= ~(_BV(PINB2));
			}
			/* TODO: Need to adjust it and add option to chose on what condition alarm should be turned on */
			
			if(500 < abs((xInit < x) ? (x - xInit) : (xInit - x)))
			{
				PORTB |= _BV(PINB2);
			}
			else
			{
				PORTB &= ~(_BV(PINB2));
			}
			
			
			/*
			for(n = 0; n < sizeof(bufforX_Init); n++)
			{
				USART_Transmit(bufforX_Init[n]);
			}
			itoa(x,bufforX,10);
			for(n = 0; n < sizeof(bufforX); n++)
			{
				USART_Transmit(bufforX[n]);
			}
			for(n = 0; n < sizeof(bufforX); n++)
			{
				bufforX[n] = " ";
			}
			*/
			
			/* TODO: Delete this line of code when project is done */
			/*
			itoa(angleX, bufforAngleX, 10);
			itoa(angleY, bufforAngleY, 10);
			for(n = 0; n < sizeof(bufforAngleX); n++)
			{
				//USART_Transmit(bufforAngleX[n]);
			}
			for(n = 0; n < sizeof(bufforAngleY); n++)
			{
				//USART_Transmit(bufforAngleY[n]);
			}
			for(n = 0; n < sizeof(bufforAngleX); n++)
			{
				bufforAngleX[n] = "";
			}
			for(n = 0; n < sizeof(bufforAngleY); n++)
			{
				bufforAngleY[n] = "";
			}
			*/
			state = ACCEL_RUN_STATE;
			
			/* TODO: Change delay time when project is done */
			_delay_ms(500);
			break; /* End of ACCEL_COMP_STATE */
			
			
		} /* End of Switch(state) */
		
		
    } /* End of while(1) loop */
	
	
	return 0; /* End of program, MM should never get here */
	
	
}

