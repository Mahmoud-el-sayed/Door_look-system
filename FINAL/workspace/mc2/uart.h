 /******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.h
 *
 * Description: Header file for the UART AVR driver
 *
 * Author: mahmoud mohamed
 *
 *******************************************************************************/
#ifndef UART_H_
#define UART_H_
/*******************************************************************************
 *                                includes                                 *
 *******************************************************************************/
#include "std_types.h"
/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/
/*******************************************************************************
 *  Enum name : e_data_bits_size
 *  Enum Description:
 *  this enum is responsible for the character size
 *  where: bit 2:0 from UCSRB Register (UCSZ 2:0)
 *  5-bit        = (value 0)
 *  6-bit        = (value 1)
 *  7-bit        = (value 2)
 *  8-bit        = (value 3)
 *  9-bit        = (value 7)
 */
typedef enum
{
	_5_BITS_SIZE,_6_BITS_SIZE,_7_BITS_SIZE,_8_BITS_SIZE,_9_BITS_SIZE=7
}e_data_bits_size;

/*******************************************************************************
 *  Enum name : e_parity_mode
 *  Enum Description:
 *  this enum is responsible for the Party mode select
 *  where: bit 4,5 from UCSRC Register  (UPM1:0)
 *  DISABLED_PARTY     = (value 0)
 *  RESERVED           = (value 2)
 *  EVEN_PARTY         = (value 3)
 *  ODD_PARTY          = (value 4)
 */
typedef enum
{
	DISABLED_PARITY,RESERVED,EVEN_PARITY,ODD_PARITY
}e_parity_mode;
/*******************************************************************************
 *  Enum name : e_stop_bit
 *  Enum Description:
 *  this enum is responsible for the STOP BIT SELECT
 *  where: bit 3 from UCSRC Register  (USBS)
 *  _1_BIT_STOP     = (value 0)
 *  _2_BIT_STOP     = (value 2)
 */
typedef enum
{
	_1_BIT_STOP,_2_BIT_STOP
}e_stop_bit;
/*******************************************************************************
 *  Structure name : s_uart_ConfigType
 *  Structure Description:
 *  this Structure is responsible for
 *  1-character size
 *  2-parity mode
 *  3-number of stop bits
 *  4-value of baud rate
 *  for dynamic Configuration
 */
typedef struct
{
	e_data_bits_size bits_size;
	e_parity_mode parity_mode;
	e_stop_bit stop_bits;
	uint32 baud_rate;
}s_uart_ConfigType;
/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_init(const s_uart_ConfigType * Config_Ptr);
/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data);
/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_recieveByte(void);
/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str);
/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *Str); // Receive until #
#endif /* UART_H_ */
