/*
 *	AccelerationSensor.c
 *
 *	Created: 16.05.2018 23:01:13
 *	Author : wyzku
 *
 *	Notes:
 *		-	define ASIC type(LIS3DH/MPU_9150) in Registers header file
 */
#include <avr/io.h>
#include <math.h>
#include <stdlib.h>
#include "function.h" 
#include "MPU_6050_REG.h"
#include <util/delay.h>	

/**********************************************
*	Macros and defines
***********************************************/



#define ACCEL_TRESHOLD		31
#define ACCEL_CONF_BITMASK	0x03
#define GYRO_CONF_BITMASK	0x0C

#define LED_ON				PORTB &= ~(_BV(PINB1))
#define LED_OFF				PORTB |= _BV(PINB1)
#define LED_TOGGLE			PORTB ^= _BV(PINB1)

#define ALARM_ON			PORTB |= _BV(PINB2)
#define ALARM_OFF			PORTB &= ~(_BV(PINB2))

#define TRUE	1U
#define FALSE	0U

/**********************************************
*	Constants
***********************************************/

const double PI = 3.141592;
const long COUNT_NO = 100;

/**********************************************
*	Global variables
***********************************************/

short int readBuffer;
short int state;
short int n;
short int accelConf;
short int gyroConf;

unsigned int tempX;
unsigned int tempY;
unsigned int tempZ;
unsigned int angleAlarmConfig;
unsigned int zAlarmCnt;
unsigned int angleXAlarmCnt;
unsigned int angleYAlarmCnt;

int count;

long x;
long y;
long z;
long xInit;
long yInit;
long zInit;

_Bool zAlarm;
_Bool angleAlarmX;
_Bool angleAlarmY;
_Bool angleXAlarm;
_Bool angleYAlarm;

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

enum accelState
{
	ACCEL_INIT_MM,
	ACCEL_CONFIG_STATE,
	ACCEL_INIT_DATA,
	ACCEL_RUN_STATE,
	ACCEL_WORKING_STATE,
	ACCEL_COMP_STATE,
};

/**********************************************
*	Source code
***********************************************/

int main(void)
{
	state = ACCEL_INIT_MM;
	zAlarm = FALSE;
	angleAlarmX = FALSE;
	angleAlarmY = FALSE;
	angleAlarmConfig = 0;
	zAlarmCnt = 0;
	angleXAlarmCnt = 0;
	angleYAlarmCnt = 0;
	
	
    while (1) 
    {
		
		switch(state)
		{
		
			
		/* State that initiate I/O and communication ports of uC */
		case ACCEL_INIT_MM:
			/* configuration of uC, I/O pins */
			DDRB |= (_BV(PINB1) | _BV(PINB2)) ;		/* configured as an output */
			DDRC &= ~(_BV(PINC0) | _BV(PINC1) | _BV(PINC2) | _BV(PINC3)); /* configured as an input */
			/* inputs pull-up configuration */
			PORTC |= 0x0F;
			PORTB |= _BV(PINB1);
			
			I2C_Init();
			
			accelConf = 0x00;
			gyroConf = 0x00;
			
			/* Read configuration for angle difference that turn alarm ON  */
			if(0 == (PINC & (_BV(PINC2) | _BV(PINC3))))
			{
				angleAlarmConfig = 2;
			}
			else if(8 == (PINC & (_BV(PINC2) | _BV(PINC3))))
			{
				angleAlarmConfig = 3;
			}
			else if(4 == (PINC & (_BV(PINC2) | _BV(PINC3))))
			{
				angleAlarmConfig = 4;
			}
			else if(12 == (PINC & (_BV(PINC2) | _BV(PINC3))))
			{
				angleAlarmConfig = 5;
			}
			
			/* Read configuration - which condition use to turn alarm ON */
			if(0 == (PINC & (_BV(PINC0) | _BV(PINC1))))
			{
				angleXAlarm = TRUE;
				angleYAlarm = TRUE;
			}
			else if(2 == (PINC & (_BV(PINC0) | _BV(PINC1))))
			{
				angleXAlarm = TRUE;
				angleYAlarm = FALSE;
			}
			else if(1 == (PINC & (_BV(PINC0) | _BV(PINC1))))
			{
				angleYAlarm = TRUE;
				angleXAlarm = FALSE;
			}
			else if(3 == (PINC & (_BV(PINC0) | _BV(PINC1))))
			{
				angleXAlarm = FALSE;
				angleYAlarm = FALSE;
			}
			
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
			
			if(0x68 == (readBuffer & ~(0x81)))
			{
				/* If communication with device is working ***********/
				for(n = 0; n < 20; n++)								 //
				{													 //
					LED_TOGGLE;										 //
					_delay_ms(100);									 //
				}													 //
				/*****************************************************/
				state = ACCEL_INIT_DATA;	
			}
			else
			{
				/* If there is fault in communication with device ****/
				for(n = 0; n < 10; n++)								 //
				{													 //
					LED_TOGGLE;										 //
					_delay_ms(500);									 //					
				}													 //
				/*****************************************************/
			}
			LED_ON;
			_delay_ms(6000);
			LED_OFF;
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
				/* Read data from MPU-6050 */
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
				/*
				if(16000 < abs((zInit < accelsTemp.z) ? (accelsTemp.z - zInit) : (zInit - accelsTemp.z)))
				{
					LED_OFF;
					ALARM_ON;
					zAlarm = TRUE;
				}
				else if(zAlarm)
				{
					if(++zAlarmCnt > 1000)
					{
						zAlarm = FALSE;
						zAlarmCnt = 0;
						ALARM_OFF;
						LED_ON;
					}
					else
					{
						// Do nothing
					}
				}
				else
				{
					// Do nothing
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
				
				angleX = (atan2((double)(x), (double)(z))/ (PI/180));
				angleY = (atan2((double)(y), (double)(z)) / (PI/180));
				
				state = ACCEL_COMP_STATE;
			}	
			break; /* End of ACCEL_WORKING_STATE case */
		
		
		case ACCEL_COMP_STATE:
			/* TODO: Need to adjust it and add option to chose on what condition alarm should be turned on */
			
			if (angleXAlarm)
			{
				if(angleAlarmConfig < abs(angleX > angleX_Init ? angleX - angleX_Init : angleX_Init - angleX))
					{
						if(++angleXAlarmCnt > 1)
						{
						ALARM_ON;
						angleAlarmX = TRUE;
						}
						else
						{
							// Do nothing
						}
					}
					else if(angleAlarmX)
					{
						ALARM_OFF;
						angleAlarmX = FALSE;
						angleXAlarmCnt = 0;
					}
					else
					{
						// Do nothing
					}
			} 
			else
			{
				// Do nothing
			}
			
			if (angleYAlarm)
			{
				if(angleAlarmConfig < abs(angleY > angleY_Init ? angleY - angleY_Init : angleY_Init - angleY))
				{
					if(++angleYAlarmCnt > 1)
					{
						ALARM_ON;
						angleAlarmY = TRUE;
					}
					else
					{
						// Do nothing
					}
				}
				else if(angleAlarmY)
				{
					ALARM_OFF;
					angleAlarmY = FALSE;
					angleYAlarmCnt = 0;
				}
				else
				{
					// Do nothing
				}
			}
			else
			{
				// Do nothing
			}

			state = ACCEL_RUN_STATE;
			
			_delay_ms(20);
			break; /* End of ACCEL_COMP_STATE */
			
			
		} /* End of Switch(state) */
		
		
    } /* End of while(1) loop */
	
	
	return 0; /* End of program, MM should never get here */
	
}

