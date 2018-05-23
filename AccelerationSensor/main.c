/*
 * AccelerationSensor.c
 *
 * Created: 16.05.2018 23:01:13
 * Author : wyzku
 */ 

#include <avr/io.h>
#include "function.h"

typedef struct 
{
	int8_t x;
	int8_t y;
	int8_t z;
}	accels_ts;

accels_ts accelsTemp, accels;

uint8_t state;
static uint8_t buffer;


int main(void)
{
	state = ACCEL_INIT_STATE;
	
    while (1) 
    {
		switch(state)
		{
		case ACCEL_INIT_STATE:
			I2C_Init();
			state = ACCEL_CONFIG_STATE;
		break;
			
		case ACCEL_CONFIG_STATE:			
			I2C_SendStartAndSelect(LIS3DH_W);
			I2C_SendByte(CTRL_REG1);
			I2C_SendStartAndSelect(LIS3DH_R);
			buffer = I2C_ReceiveDataByte_NACK();
			I2C_Stop();
	
			buffer |= _BV(LPen) | _BV(ODR2) | _BV(ODR0);
			
			I2C_SendStartAndSelect(LIS3DH_W);
			I2C_SendByte(CTRL_REG1);
			I2C_SendByte(buffer);
			I2C_Stop();
			
			I2C_SendStartAndSelect(LIS3DH_W);
			I2C_SendByte(CTRL_REG4);
			I2C_SendStartAndSelect(LIS3DH_R);
			buffer = I2C_ReceiveDataByte_NACK();
			I2C_Stop();
			
			buffer |= _BV(BDU) | _BV(FS1);
			
			I2C_SendStartAndSelect(LIS3DH_W);
			I2C_SendByte(CTRL_REG1);
			I2C_SendByte(buffer);
			I2C_Stop();
			
			state = ACCEL_RUN_STATE;
		break;
			
		case ACCEL_RUN_STATE:
			I2C_SendStartAndSelect(LIS3DH_W);
			I2C_SendByte(OUT_X_L | 0x80);
			I2C_SendStartAndSelect(LIS3DH_R);
			accelsTemp.x = I2C_ReceiveDataBytes_ACK();
			accelsTemp.y = I2C_ReceiveDataBytes_ACK();
			accelsTemp.z = I2C_ReceiveDataByte_NACK();
			I2C_Stop();
			
			state = ACCEL_WORKING_STATE;
		break;
			
		case ACCEL_WORKING_STATE:
			if((accelsTemp.x - accels.x) > 32)
			{
				PORTB |= _BV(5);
			}
			else if((accelsTemp.y - accels.y) > 32)
			{
				PORTB |= _BV(5);
			}
			else if((accelsTemp.z - accels.z) > 32)
			{
				PORTB |= _BV(5);
			}
			else
			{
				accels.x = accelsTemp.x;
				accels.y = accelsTemp.y;
				accels.z = accelsTemp.z;
				
				PORTB &= ~_BV(5);
			}
			
			state = ACCEL_RUN_STATE;
		break;
		}
    }
}

