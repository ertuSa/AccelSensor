#ifndef _REGISTERS_H
#define _REGISTERS_H

// ASIC model

#define		LIS3DH
//#define		MPU_9150

// LIS3DH registers
#ifdef LIS3DH

#define LIS3DH_R			0x31
#define LIS3DH_W			0x30

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
#define OUT_X_H				0x29
#define OUT_Y_L				0x2A
#define OUT_Y_H				0x2B
#define OUT_Z_L				0x2C
#define OUT_Z_H				0x2E

#endif

// MPU-9150 registers
#ifdef MPU_9150

#define MPU_9150_R			0xD8
#define MPU_9150_W			0x68

#define CONFIG_REG			0x1A
#define DLPF_CFG0			0
#define DLPF_CFG1			1
#define DLPF_CFG2			2
#define EXT_SYNC_SET0		3
#define EXT_SYNC_SET1		4
#define EXT_SYNC_SET2		5

#define GYRO_CONFIG_REG		0x1B
#define FS_SEL0				3
#define FS_SEL1				4
#define ZG_ST				5
#define YG_ST				6
#define XG_ST				7

#define ACCEL_CONFIG_REG	0x1C
#define AFS_SEL0			3
#define AFS_SEL1			4
#define ZA_ST				5
#define YA_ST				6
#define XA_ST				7

#define FIFO_EN_REG			0x23
#define SLVO_FIFO_EN		0
#define SLV1_FIFO_EN		1
#define SLV2_FIFO_EN		2
#define ACCEL_FIFO_EN		3
#define ZG_FIFO_EN			4
#define YG_FIFO_EN			5
#define XG_FIFO_EN			6
#define TEMP_FIFO_EN		7

#define INT_ENABLE_REG		0x38
#define DATA_RDY_EN			0
#define I2C_MST_INT_EN		3
#define FIFO_OFLOW_EN		4

#define INT_STATUS_REG		0x3A
#define DATA_RDY_INT		0
#define I2C_MST_INT			3
#define FIFO_OFLOW_INT		4

#define ACCEL_XOUT_H_REG	0x3B
#define ACCEL_XOUT_L_REG	0x3C
#define ACCEL_YOUT_H_REG	0x3D
#define ACCEL_YOUT_L_REG	0x3E
#define ACCEL_ZOUT_H_REG	0x3F
#define ACCEL_ZOUT_L_REG	0x40

#define GYRO_XOUT_H_REG		0x43
#define GYRO_XOUT_L_REG		0x44
#define GYRO_YOUT_H_REG		0x45
#define GYRO_YOUT_L_REG		0x46
#define GYRO_ZOUT_H_REG		0x47
#define GYRO_ZOUT_L_REG		0x48

#define USER_CTRL_REG		0x6A
#define SIG_COND_RESET		0
#define I2C_MST_RESET		1
#define FIFO_RESET			2
#define I2C_IF_DIS			4		//always write this bit to 0 
#define I2C_MST_EN			5
#define FIFO_EN				6

#define PWR_MGMT_1_REG		0x6B
#define CLKSEL0				0
#define CLKSEL1				1
#define CLKSEL2				2
#define TEMP_DIS			3
#define CYCLE				5
#define SLEEP				6
#define DEVICE_RESET		7

#define PWR_MGMT_2_REG		0x6C
#define STBY_ZG				0
#define STBY_YG				1
#define STBY_XG				2
#define STBY_ZA				3
#define STBY_YA				4
#define STBY_XA				5
#define LP_WAKE_CTRL0		6
#define LP_WAKE_CTRL1		7

#define FIFO_COUNT_H_REG	0x72
#define FIFO_COUNT_L_REG	0x73

#define FIFO_R_W_REG		0x74

#define WHO_AM_I_REG		0x75

#endif

#endif