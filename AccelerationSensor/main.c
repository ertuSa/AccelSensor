/*
 *	AccelerationSensor.c
 *
 *	Created: 16.05.2018 23:01:13
 *	Author : wyzku
 *
 *	Notes:
 *		-	define ASIC type(LIS3DH/MPU_9150) in Registers header file
 */ 
#include "Registers.h"		

#define F_CPU				16000000UL
#define MYUBRR				0x67 /* decimal 103 = baud rate 9600 */

#define ACCEL_TRESHOLD		31

#include <avr/io.h>
#include <util/delay.h>
#include "function.h"

int16_t temp, counter;
int8_t readBuffer;
int16_t tempX, tempY, tempZ;
int8_t state, writeBuffer;
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
		
		#ifdef LIS3DH					
			I2C_SendStartAndSelect(LIS3DH_W);	//
			I2C_SendByte(CTRL_REG1 | ADR_INC_MASK);
			I2C_SendByte(0x5F);		// CTRL_REG1 = 0x57
			I2C_SendByte(0x00);		// CTRL_REG2 = 0x00
			I2C_SendByte(0x00);		// CTRL_REG3 = 0x00
			I2C_SendByte(0x80);		// CTRL_REG4 = 0x48
			I2C_SendByte(0x00);		// CTRL_REG5 = 0x00
			I2C_SendByte(0x00);		// CTRL_REG6 = 0x00
			I2C_Stop();
		#else
			I2C_SendStartAndSelect(MPU_9150_W);
			I2C_SendByte(INT_ENABLE_REG);
			I2C_SendByte(0x01);
			I2C_Stop();
			
			I2C_SendStartAndSelect(MPU_9150_W);
			I2C_SendByte(SMPRT_DIV_REG);
			I2C_SendByte(0x07);
			I2C_SendByte(0x00);
			I2C_SendByte(0x00);
			I2C_SendByte(0x00);
			I2C_Stop();
			
			I2C_SendStartAndSelect(MPU_9150_W);
			I2C_SendByte(PWR_MGMT_1_REG);
			I2C_SendByte(0x09);
			I2C_Stop();
			
			
			
			
		#endif
			
			/* Checking the communication with device */
		#ifdef LIS3DH
			I2C_SendStartAndSelect(LIS3DH_W);
			I2C_SendByte(WHO_AM_I);
			I2C_Start();
			I2C_SendByte(LIS3DH_R);
			readBuffer = I2C_ReceiveDataByte_NACK();
		#else
			I2C_SendStartAndSelect(MPU_9150_W);
			I2C_SendByte(WHO_AM_I_REG);
			I2C_SendStartAndSelect(MPU_9150_R);
			readBuffer = I2C_ReceiveDataByte_NACK();
		#endif	
			I2C_Stop();
		#ifdef LSI3DH
			if(0x33 == readBuffer)
		#endif
			USART_Transmit(readBuffer);
			if(0x68 == (readBuffer & ~(0x81)))
			{
				//TODO: This part of code need to be deleted before release
				/* For debugging only ***********/
				for(uint8_t x = 0; x < 20; x++)	//
				{								//
					PORTB ^= _BV(PB5);			//
					_delay_ms(100);				//
				}								//
				/********************************/
				#ifdef LIS3DH
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
					//accels.x = (accels.x << 8);
					//accels.x |= I2C_ReceiveDataBytes_ACK();
					temp = I2C_ReceiveDataBytes_ACK();
					accels.y = I2C_ReceiveDataBytes_ACK();
					//accels.y = (accels.y << 8);
					//accels.y |= I2C_ReceiveDataBytes_ACK();
					//accels.z = I2C_ReceiveDataBytes_ACK();
					//accels.z = (accels.z << 8);
					temp = I2C_ReceiveDataBytes_ACK();
					accels.z = I2C_ReceiveDataByte_NACK();
					I2C_Stop();
				}
				#else
				I2C_SendStartAndSelect(MPU_9150_W);
				I2C_SendByte(INT_STATUS_REG);
				I2C_SendStartAndSelect(MPU_9150_R);
				if(_BV(DATA_RDY_INT) & I2C_ReceiveDataByte_NACK())
				{
					I2C_SendStartAndSelect(MPU_9150_W);
					I2C_SendByte(GYRO_XOUT_H_REG);
					I2C_SendStartAndSelect(MPU_9150_R);
					accels.x = (I2C_ReceiveDataBytes_ACK() << 8);
					accels.x |= I2C_ReceiveDataByte_NACK();
				}
				#endif
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
		#ifdef LIS3DH
			I2C_SendStartAndSelect(LIS3DH_W);
			I2C_SendByte(STATUS_REG_AUX);
			I2C_SendStartAndSelect(LIS3DH_R);
			readBuffer = I2C_ReceiveDataByte_NACK();
			if(_BV(DA321) & readBuffer)
		#else
			I2C_SendStartAndSelect(MPU_9150_W);
			I2C_SendByte(INT_STATUS_REG);
			I2C_SendStartAndSelect(MPU_9150_R);
			readBuffer = I2C_ReceiveDataByte_NACK();
			if(_BV(DATA_RDY_INT) & readBuffer)
		#endif
			{
			#ifdef LIS3DH	
				I2C_SendStartAndSelect(LIS3DH_W);
				I2C_SendByte(OUT_X_H | 0x80);
				I2C_SendStartAndSelect(LIS3DH_R);
				accelsTemp.x = I2C_ReceiveDataBytes_ACK();
				//accelsTemp.x = (accelsTemp.x << 8);
				//accelsTemp.x |= I2C_ReceiveDataBytes_ACK();
				temp = I2C_ReceiveDataBytes_ACK();
				accelsTemp.y = I2C_ReceiveDataBytes_ACK();
				//accelsTemp.y = (accelsTemp.y << 8);
				//accelsTemp.y |= I2C_ReceiveDataBytes_ACK();
				temp = I2C_ReceiveDataBytes_ACK();
				//accelsTemp.z = I2C_ReceiveDataBytes_ACK();
				//accelsTemp.z = (accelsTemp.z << 8);
				accelsTemp.z = I2C_ReceiveDataByte_NACK();
			#else
				I2C_SendStartAndSelect(MPU_9150_W);
				I2C_SendByte(GYRO_XOUT_H_REG);
				I2C_SendStartAndSelect(MPU_9150_R);
				accelsTemp.x = (I2C_ReceiveDataBytes_ACK() << 8);
				accelsTemp.x |= I2C_ReceiveDataByte_NACK();
			#endif	
				I2C_Stop();
				
				state = ACCEL_WORKING_STATE;
				
			}	
		break;
			
		/* State that compare acceleration values from memory and values get from acceleration sensor */	
		case ACCEL_WORKING_STATE:
		
			if(accels.x > accelsTemp.x)
			{
				temp = accels.x - accelsTemp.x;
				if(temp < 0) temp *= -1;
			}
			else
			{
				temp = accelsTemp.x - accels.x;
				if(temp < 0) temp *= -1;
			}
			if(temp > 1000)
			{
				PORTB |= _BV(5);
			}
			else
			{
				PORTB &= ~_BV(5);
			}
			//USART_Transmit((uint8_t)(accelsTemp.x >> 8));
			USART_Transmit((int8_t)(temp >> 8));
			USART_Transmit((int8_t)temp);
			//temp = 0x5A;
			//USART_Transmit(temp);
			//USART_Transmit((uint8_t)(accelsTemp.y >> 8));
			//USART_Transmit(accelsTemp.y);
			//USART_Transmit(temp);
			//USART_Transmit((uint8_t)(accelsTemp.z >> 8));
			//USART_Transmit(accelsTemp.z);
			//USART_Transmit(temp);
			//USART_Transmit(temp);
			
			//USART_Transmit("\n");
			_delay_ms(250);
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

