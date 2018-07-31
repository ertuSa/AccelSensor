#ifndef _MPU_6050_REG_H
#define _MPU_6050_REG_H

/* Head file containing MPU-6050 register address */

/* Address of MPU-6050 ASIC */
#define MPU_6050_R			0xD1
#define MPU_6050_W			0xD0

#define SMPRT_DIV_REG		0x19	// Register used to set sample rate(Gyro rate/1 + SMPRT_DIV_REG).
									// (regmap file page 11)

#define CONFIG_REG			0x1A	// Register used to set FSYNC pin sampling and the Digital
#define DLPF_CFG0			0		// Low Pass Filter(DLPF). (regmap file page 13)
#define DLPF_CFG1			1		
#define DLPF_CFG2			2		
#define EXT_SYNC_SET0		3
#define EXT_SYNC_SET1		4
#define EXT_SYNC_SET2		5

#define GYRO_CONFIG_REG		0x1B	// Register used to trigger gyro self-test and configure the gyro
#define FS_SEL0				3		// full scale range. (regmap file page 14)
#define FS_SEL1				4
#define ZG_ST				5
#define YG_ST				6
#define XG_ST				7

#define ACCEL_CONFIG_REG	0x1C	// Register used to trigger accel self-test and configure the accel
#define AFS_SEL0			3		// full scale range. (regmap file page 15)
#define AFS_SEL1			4
#define ZA_ST				5
#define YA_ST				6
#define XA_ST				7

#define FIFO_EN_REG			0x23	// 
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