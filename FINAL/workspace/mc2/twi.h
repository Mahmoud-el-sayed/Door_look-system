 /******************************************************************************
 *
 * Module: TWI(I2C)
 *
 * File Name: twi.h
 *
 * Description: Header file for the TWI(I2C) AVR driver
 *
 * Author: mahmoud mohammed
 *
 *******************************************************************************/ 

#ifndef TWI_H_
#define TWI_H_

#include "std_types.h"

/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/

/* I2C Status Bits in the TWSR Register */
#define TWI_START         0x08 /* start has been sent */
#define TWI_REP_START     0x10 /* repeated start */
#define TWI_MT_SLA_W_ACK  0x18 /* Master transmit ( slave address + Write request ) to slave + ACK received from slave. */
#define TWI_MT_SLA_R_ACK  0x40 /* Master transmit ( slave address + Read request ) to slave + ACK received from slave. */
#define TWI_MT_DATA_ACK   0x28 /* Master transmit data and ACK has been received from Slave. */
#define TWI_MR_DATA_ACK   0x50 /* Master received data and send ACK to slave. */
#define TWI_MR_DATA_NACK  0x58 /* Master received data but doesn't send ACK to slave. */


/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/

/*******************************************************************************
 *
 *  Enum name : e_pre_scaler
 *  Enum Description:
 *  this enum is responsible for prescaler value to use it in bit rate formula for SCL frequency
 *  where: bits 1:0 from TWSR Register (TWPS 1:0)
 */
typedef enum
{
	PRESCCALER_1,PRESCCALER_4,PRESCCALER_16,PRESCCALER_64
}e_pre_scaler;

/*******************************************************************************
 *  Enum name : e_mode_rate
 *  Enum Description:
 *  this enum is responsible for Rate value to use it in bit rate formula for SCL frequency
 */
typedef enum
{
	NORMAL_MODE=100000,FAST_MODE=400000,FAST_MODE_PLUS=1000000,HIGH_SPEED_MODE=3400000
}e_mode_rate;

/*******************************************************************************
 *  Structure name : s_TWI_ConfigType
 *  Structure Description:
 *  this Structure is responsible for
 *  1-Address used in case this MC is a slave device
 *  2-prescaler value
 *  3-type of mode
 *  for dynamic Configuration
 */
typedef struct
{
	uint8 MY_ADDRESS;
	e_pre_scaler pre_scaler;
	e_mode_rate mode_rate;

}s_TWI_ConfigType;
/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description :
 * Functional responsible for Initialize the TWI by:
 * 1. set the prescaler value
 * 2. set TWBR value
 * 3. enable TWI
 */
void TWI_init(const s_TWI_ConfigType * Config_Ptr);
/*
 * Description :
 * Functional responsible for start bit for TWI by
 */
void TWI_start(void);


/*
 * Description :
 * Functional responsible for stop bit for TWI by
 */
void TWI_stop(void);


/*
 * Description :
 * Functional responsible for write byte with TWI
 */
void TWI_writeByte(uint8 data);

/*
 * Description :
 * Functional responsible for read byte with ack
 */
uint8 TWI_readByteWithACK(void);

/*
 * Description :
 * Functional responsible for read byte with Nack
 */
uint8 TWI_readByteWithNACK(void);

/*get status of TWI  */
uint8 TWI_getStatus(void);


#endif /* TWI_H_ */
