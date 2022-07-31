 /******************************************************************************
 *
 * Module: TWI(I2C)
 *
 * File Name: twi.h
 *
 * Description: Source file for the TWI(I2C) AVR driver
 *
 * Author: mahmoud mohammed
 *
 *******************************************************************************/

/*******************************************************************************
 *                                includes                                 *
 *******************************************************************************/
#include "twi.h"
#include "common_macros.h"
#include <avr/io.h>

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description :
 * Functional responsible for Initialize the TWI by:
 * 1. set the prescaler value
 * 2. set TWBR value
 * 3. enable TWI
 */
void TWI_init(const s_TWI_ConfigType * Config_Ptr)
{
	uint8 M_pre_scaler;
    /* set the prescaler value  */
	TWSR =Config_Ptr->pre_scaler;

	/*calculate value of PRESCCALER for TWBR equation */
	if (Config_Ptr->pre_scaler==PRESCCALER_1)
	{
		M_pre_scaler=1;
	}
	else if (Config_Ptr->pre_scaler==PRESCCALER_4)
	{
		M_pre_scaler=4;
	}
	else if (Config_Ptr->pre_scaler==PRESCCALER_16)
	{
		M_pre_scaler=16;
	}
	else
	{
		M_pre_scaler=64;
	}

	/*calculate variable what i need to storage it in TWBR register*/
	TWBR=(uint8)((F_CPU/ ((Config_Ptr->mode_rate) * 2) - 8)/M_pre_scaler);


    /* Two Wire Bus address my address if any master device want to call me:  (used in case this MC is a slave device)
      *
      *set the address by shift to left one time to set it in bits 7:1
      * bit 0: General Call Recognition: Off
      */
    TWAR = (Config_Ptr->MY_ADDRESS)<<1;

    TWCR = (1<<TWEN); /* enable TWI */

}
/*
 * Description :
 * Functional responsible for start bit for TWI by
 */
void TWI_start(void)
{
    /* 
	 * Clear the TWINT flag before sending the start bit TWINT=1
	 * send the start bit by TWSTA=1
	 * Enable TWI Module TWEN=1 
	 */
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    
    /* Wait for TWINT flag set in TWCR Register (start bit is send successfully) */
    while(BIT_IS_CLEAR(TWCR,TWINT));
}
/*
 * Description :
 * Functional responsible for stop bit for TWI by
 */
void TWI_stop(void)
{
    /* 
	 * Clear the TWINT flag before sending the stop bit TWINT=1
	 * send the stop bit by TWSTO=1
	 * Enable TWI Module TWEN=1 
	 */
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}
/*
 * Description :
 * Functional responsible for write byte with TWI
 */
void TWI_writeByte(uint8 data)
{
    /* Put data On TWI data Register */
    TWDR = data;
    /* 
	 * Clear the TWINT flag before sending the data TWINT=1
	 * Enable TWI Module TWEN=1 
	 */ 
    TWCR = (1 << TWINT) | (1 << TWEN);
    /* Wait for TWINT flag set in TWCR Register(data is send successfully) */
    while(BIT_IS_CLEAR(TWCR,TWINT));
}
/*
 * Description :
 * Functional responsible for read byte with ack
 */
uint8 TWI_readByteWithACK(void)
{
	/* 
	 * Clear the TWINT flag before reading the data TWINT=1
	 * Enable sending ACK after reading or receiving data TWEA=1
	 * Enable TWI Module TWEN=1 
	 */ 
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    /* Wait for TWINT flag set in TWCR Register (data received successfully) */
    while(BIT_IS_CLEAR(TWCR,TWINT));
    /* Read Data */
    return TWDR;
}

/*
 * Description :
 * Functional responsible for read byte with Nack
 */
uint8 TWI_readByteWithNACK(void)
{
	/* 
	 * Clear the TWINT flag before reading the data TWINT=1
	 * Enable TWI Module TWEN=1 
	 */
    TWCR = (1 << TWINT) | (1 << TWEN);
    /* Wait for TWINT flag set in TWCR Register (data received successfully) */
    while(BIT_IS_CLEAR(TWCR,TWINT));
    /* Read Data */
    return TWDR;
}
/*get status of TWI  */
uint8 TWI_getStatus(void)
{
    uint8 status;
    /* masking to eliminate first 3 bits and get the last 5 bits (status bits) */
    status = TWSR & 0xF8;
    return status;
}
