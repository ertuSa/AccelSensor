/*
 * AccelerationSensor.c
 *
 * Created: 16.05.2018 23:01:13
 * Author : wyzku
 */ 
#define F_CPU				16000000UL
#define ACCEL_TRESHOLD		31

#include <avr/io.h>
#include <util/delay.h>
#include "function.h"



typedef struct 
{
	int8_t x;
	int8_t y;
	int8_t z;
}	accels_ts;

accels_ts accelsTemp;

accels_ts accels = { 127, 127, 127};

uint8_t state;
static int8_t readBuffer;
static uint8_t writeBuffer;


int main(void)
{
	state = ACCEL_INIT_STATE;
	DDRB |= _BV(5);
	//DDRC |= _BV(PC0);
	DDRC |= _BV(PC4) | _BV(PC5);
	PRR &= ~_BV(PRTWI);
	
    while (1) 
    {
		
		
		//PORTB ^= _BV(PB5);
		//_delay_ms(1000);
		
		switch(state)
		{
		case ACCEL_INIT_STATE:
			I2C_Init();
			state = ACCEL_CONFIG_STATE;
			_delay_ms(100);
			for (int x = 0; x < 4; x++)
			{
				_delay_ms(250);
				PORTB ^= _BV(5);
			}
			
		break;
			
		case ACCEL_CONFIG_STATE:					
			I2C_SendStartAndSelect(LIS3DH_W);
			I2C_SendByte(CTRL_REG1 | ADR_INC_MASK);
			I2C_SendByte(0x5F);		// CTRL_REG1 = 0x5F
			I2C_SendByte(0x00);		// CTRL_REG2 = 0x00
			I2C_SendByte(0x00);		// CTRL_REG3 = 0x00
			I2C_SendByte(0x00);		// CTRL_REG4 = 0x80
			I2C_SendByte(0x00);		// CTRL_REG5 = 0x00
			I2C_SendByte(0x00);		// CTRL_REG6 = 0x00
			I2C_Stop();
			
			I2C_SendStartAndSelect(LIS3DH_W);
			I2C_SendByte(WHO_AM_I);
			I2C_Start();
			I2C_SendByte(LIS3DH_R);
			readBuffer = I2C_ReceiveDataByte_NACK();
			I2C_Stop();
			if(0x33 == readBuffer)
			{
				for(uint8_t x = 0; x < 20; x++)
				{
					PORTB ^= _BV(PB5);
					_delay_ms(100);
				}
			}
			else
			{
				PORTB |= _BV(PB5);
				_delay_ms(2000);
			}
			
			I2C_SendStartAndSelect(LIS3DH_W);
			I2C_SendByte(STATUS_REG_AUX);
			I2C_SendStartAndSelect(LIS3DH_R);
			readBuffer = I2C_ReceiveDataByte_NACK();
			if(_BV(DA321) & readBuffer)
			{
				I2C_SendStartAndSelect(LIS3DH_W);
				I2C_SendByte(OUT_X_L | 0x80);
				I2C_SendStartAndSelect(LIS3DH_R);
				accels.x = I2C_ReceiveDataBytes_ACK();
				accels.y = I2C_ReceiveDataBytes_ACK();
				accels.z = I2C_ReceiveDataByte_NACK();
				I2C_Stop();
			}
			
			state = ACCEL_RUN_STATE;
		break;
			
		case ACCEL_RUN_STATE:
			I2C_SendStartAndSelect(LIS3DH_W);
			I2C_SendByte(STATUS_REG_AUX);
			I2C_SendStartAndSelect(LIS3DH_R);
			readBuffer = I2C_ReceiveDataByte_NACK();
			if(_BV(DA321) & readBuffer)
			{
				I2C_SendStartAndSelect(LIS3DH_W);
				I2C_SendByte(OUT_X_L | 0x80);
				I2C_SendStartAndSelect(LIS3DH_R);
				accelsTemp.x = I2C_ReceiveDataBytes_ACK();
				accelsTemp.y = I2C_ReceiveDataBytes_ACK();
				accelsTemp.z = I2C_ReceiveDataByte_NACK();
				I2C_Stop();
				
				state = ACCEL_WORKING_STATE;
			}
			
			
		break;
			
		case ACCEL_WORKING_STATE:
			if(((accelsTemp.x * (-1)) - accels.x) > ACCEL_TRESHOLD )
			{
				for(uint8_t x = 0; x < 20; x++)
				{
					PORTB ^= _BV(PB5);
					_delay_ms(100);
				}
				PORTB &= ~_BV(PB5);
			}
			else if(((accelsTemp.y * (-1)) - accels.y) > ACCEL_TRESHOLD )
			{
				for(uint8_t x = 0; x < 20; x++)
				{
					PORTB ^= _BV(PB5);
					_delay_ms(100);
				}
				PORTB &= ~_BV(PB5);
			}
			else if(((accelsTemp.z * (-1)) - accels.z) > ACCEL_TRESHOLD )
			{
				for(uint8_t x = 0; x < 20; x++)
				{
					PORTB ^= _BV(PB5);
					_delay_ms(100);
				}
				PORTB &= ~_BV(PB5);
			}
			else
			{
				
			}
			
			state = ACCEL_RUN_STATE;
		break;
		}
    }
}

