/*
 * AccelerationSensor.c
 *
 * Created: 16.05.2018 23:01:13
 * Author : wyzku
 */ 

#define F_CPU				16000000UL
#define MYUBRR				0x67 /* decimal 103 = baud rate 9600 */

#define ACCEL_TRESHOLD		31

#include <avr/io.h>
#include <util/delay.h>
#include "function.h"

int8_t temp, readBuffer;
uint8_t state, writeBuffer;
accels_ts accelsTemp;

accels_ts accels = {0, 0, 0};

int main(void)
{
	state = ACCEL_INIT_STATE;
	
    while (1) 
    {
		
		switch(state)
		{
			
		/* State that initiate I/O and communication ports of uC */
		case ACCEL_INIT_STATE:
			/* configuration of uC, I/O pins */
			DDRB |= _BV(5);
			//DDRC |= _BV(PC4) | _BV(PC5);
			PRR &= ~( _BV(PRTWI) | _BV(PRUSART0) );
			/* Initialization of USART communication */
			USART_Init(MYUBRR);
			/* Initialization of I2C communication */
			I2C_Init();
			
			/* Changing the state of state machine */
			state = ACCEL_CONFIG_STATE;
			
			//TODO: This part of code need to be deleted before release
			/* For debugging only ***********/ 
			_delay_ms(100);					//
			for (int x = 0; x < 4; x++)		//
			{								//
				_delay_ms(250);				//
				PORTB ^= _BV(5);			//
			}								//
			/********************************/
		break;
		
		/* State that configure slave device */	
		case ACCEL_CONFIG_STATE:					
			I2C_SendStartAndSelect(LIS3DH_W);	//
			I2C_SendByte(CTRL_REG1 | ADR_INC_MASK);
			I2C_SendByte(0x5F);		// CTRL_REG1 = 0x5F
			I2C_SendByte(0x00);		// CTRL_REG2 = 0x00
			I2C_SendByte(0x00);		// CTRL_REG3 = 0x00
			I2C_SendByte(0x00);		// CTRL_REG4 = 0x80
			I2C_SendByte(0x00);		// CTRL_REG5 = 0x00
			I2C_SendByte(0x00);		// CTRL_REG6 = 0x00
			I2C_Stop();
			
			/* Checking the communication with device */
			I2C_SendStartAndSelect(LIS3DH_W);
			I2C_SendByte(WHO_AM_I);
			I2C_Start();
			I2C_SendByte(LIS3DH_R);
			readBuffer = I2C_ReceiveDataByte_NACK();
			I2C_Stop();
			if(0x33 == readBuffer)
			{
				//TODO: This part of code need to be deleted before release
				/* For debugging only ***********/
				for(uint8_t x = 0; x < 20; x++)	//
				{								//
					PORTB ^= _BV(PB5);			//
					_delay_ms(100);				//
				}								//
				/********************************/
				
				/* Get acceleration values from each axis */
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
			}
			else
			{
				//TODO: This part of code need to be deleted before release
				/* For debugging only ***********/
				PORTB |= _BV(PB5);				//
				_delay_ms(2000);				//
				/********************************/
			}
			
			
		break;
			
		/* State that control get acceleration values from slave device */
		case ACCEL_RUN_STATE:
			I2C_SendStartAndSelect(LIS3DH_W);
			I2C_SendByte(STATUS_REG_AUX);
			I2C_SendStartAndSelect(LIS3DH_R);
			readBuffer = I2C_ReceiveDataByte_NACK();
			if(_BV(DA321) & readBuffer)
			{
				I2C_SendStartAndSelect(LIS3DH_W);
				I2C_SendByte(OUT_X_H | 0x80);
				I2C_SendStartAndSelect(LIS3DH_R);
				accelsTemp.x = I2C_ReceiveDataBytes_ACK();
				temp = I2C_ReceiveDataBytes_ACK();
				accelsTemp.y = I2C_ReceiveDataBytes_ACK();
				temp = I2C_ReceiveDataBytes_ACK();
				accelsTemp.z = I2C_ReceiveDataByte_NACK();
				
				state = ACCEL_WORKING_STATE;
			}	
		break;
			
		/* State that compare accel values from memory and values get from acceleration sensor */	
		case ACCEL_WORKING_STATE:
			USART_Transmit(accelsTemp.x);
			USART_Transmit(accelsTemp.y);
			USART_Transmit(accelsTemp.z);
			//USART_Transmit("\n");
			_delay_ms(1000);
			/*if(((accelsTemp.x * (-1)) - accels.x) > ACCEL_TRESHOLD )
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
				
			}*/
			
			state = ACCEL_RUN_STATE;
		break;
		}
    }
}

