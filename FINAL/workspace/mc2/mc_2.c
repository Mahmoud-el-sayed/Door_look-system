/******************************************************************************
 *
 * Module: main
 *
 * File Name: mc2.c
 *
 * Description: main file for microcontroller 1
 *
 * Author: Mahmoud Mohamed
 *
 *******************************************************************************/
/*******************************************************************************
 *                                includes                                 *
 *******************************************************************************/
/* define frequency of processor to 8MGZ */
#define F_CPU 8000000UL
#include <util/delay.h> /*to use (_delay_ms(value)) */
#include"timer0.h"
#include <avr/io.h>
#include "buzzer.h"
#include"uart.h"
#include"dc_motor.h"
#include"std_types.h"
#include "external_eeprom.h"
#include "twi.h"
/*******************************************************************************
 *                                macros                                *
 *******************************************************************************/
#define MC1_READY      0x20
#define MC2_READY      0x10
/*/addrees of first element of password will store in 0x0014 in EEPROM*/
#define address_in_eeprom 0x0014
#define PASS_SIZE             5  /*refer to size of password*/
/*for one second :need 64 overflow (interrupt)*/
#define WRONG_PASSWORD 0
#define TRUE_PASSWORD  1
/*for one second :need 64 overflow (interrupt)*/
/* why 32 : (1024/(8*10^6))*250*(10^3)
 * where  1024   :  prescaler
 *        8*10^6 :  F_cpu
 *        10^3   :  one second
 *        250    : number of interrupts
 */
#define NUMBER_OF_OVERFLOWS_for_1_SECOND 32
/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description: Function for initialize  all drivers
 *  1)Enable global interrupts
 *  2)dc_motor
 *  3)UART
 */
void init_microcontroller(void);
/*
 * Description: Function to receive the password from microcontroller1 using UART
 * [Args] :
 *         [in]   : pointer to array where I will store password in (local array)
 */
void recieve_password_using_uart(uint8 * passArray_ptr);
/*
 * Description: Function to interfacing with micro1
 *              receive password,sent true_password message to micro1 or not
 *              then receive option_massage from micro1
 * [Args] :
 *         [in]   : pointer to array where I will store password in (local)
 */
void options(uint8 * passArray_ptr);
/*
 * Description: Function to store the password in EEPROM
 * [Args] :
 *         [in]   : pointer to array where I will store password in (local array)
 */
void store_password_in_EEPROM(uint8 * passArray_ptr);
/*
 * Description: Function to COMPARE the password in EEPROM WITH NEW PASSWORD
 * [Args] :
 *         [in]   : pointer to array where I will store password in (local array)
 *         [out]   : TRUE_PASSWORD
 *                  OR  WRONG_PASSWORD
 */
uint8 check_password(uint8 * passArray_ptr);
/*
 * Description:Function for
 *           1) MAKE the buzzer be ON (60 seconds)
 *              when user enter password three times wrong
 *              by set timer0 to calculate 60 seconds
 *           2) set timer call (wrong_password_off) after counting 60 seconds
 */
void wrong_password_on(void);
/*
 * Description: Function for
 *         1) turn on motor with clock_wise for 30 second
 *            when user enter password and choose option '+'
 *            by set timer0 to calculate 33 seconds
 *         2) set timer call (motor_off) after counting 60 seconds
 */
void motor_on(void);
/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/
uint16 g_tick=0;/* Global variable to counting times of timer *  */
/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description: Function for call back for timer0
 * to deinit timer0 and turn off buzzer
 */
void wrong_password_off(void)
{
	g_tick++;/*if NUMBER_OF_OVERFLOWS_for_1_SECOND occur so counter will added by 1 */
	if (g_tick==NUMBER_OF_OVERFLOWS_for_1_SECOND*60)
	{
		BUZZER_off(); /*turn off buzzer */
		g_tick=0;/*clear counter to count from 0 again*/
		TIMER_0_deInit();/*turn off timer*/
	}
}
/*
 * Description: Function for call back for timer0
 * to deinit timer0 and control motor
 */
void motor_off(void)
{
	g_tick++;/*if NUMBER_OF_OVERFLOWS_for_1_SECOND occur so counter will added by 1 */
	if (g_tick==NUMBER_OF_OVERFLOWS_for_1_SECOND*15)
	{
		DcMotor_Rotate(DC_MOTOR_STOP);/*make motor off*/
	}
	if (g_tick==NUMBER_OF_OVERFLOWS_for_1_SECOND*18)
	{

		DcMotor_Rotate(DC_MOTOR_ACW);/*make motor on and anti_clockwise*/
	}
	if (g_tick==NUMBER_OF_OVERFLOWS_for_1_SECOND*33)
	{
		TIMER_0_deInit();/*turn off timer*/
		DcMotor_Rotate(DC_MOTOR_STOP);/*make motor off*/
		g_tick=0;/*clear counter to count from 0 again*/

	}
}
/*
 * Description: main Function
 *  1)initialize  all drivers
 *  2)receive password at first time
 *  3)store password in EEPROM
 *  4)in while (1) : options function
 */
int main(void)
{
	uint8 pass_arr[PASS_SIZE]; /*to hold password */

	/*initialize  all drivers */
	init_microcontroller();
	/*receive password */
	recieve_password_using_uart(pass_arr);
	/*store password in EEPROM*/
	store_password_in_EEPROM(pass_arr);
	while(1)
	{
		options(pass_arr);/*call option function*/
	}

}
/*
 * Description: Function for initialize  all drivers
 *  1)Enable global interrupts
 *  2)dc_motor
 *  3)UART
 */
void init_microcontroller(void)
{
	/*Enable global interrupts in MC by setting the I-Bit.*/
	SREG |= (1<<7);
	/*initialize the MOTOR*/
	DcMotor_Init();
	/*initialize the UART*/
	s_uart_ConfigType conf_1={_8_BITS_SIZE,DISABLED_PARITY,_1_BIT_STOP,9600};
	UART_init(&conf_1);
	/* Initialize the TWI/I2C Driver */
	const s_TWI_ConfigType  Config={0x01,PRESCCALER_1,FAST_MODE};
	TWI_init(&Config);
}
/*
 * Description: Function to receive the password from microcontroller1 using UART
 * [Args] :
 *         [in]   : pointer to array where I will store password in (local array)
 */
void recieve_password_using_uart(uint8 * passArray_ptr)
{
	uint8 loop_count; /*counter to use it in for_loop*/

	/*you can delete this check */
	/*for checking that two microcontrollers working in same time */
	while(UART_recieveByte() != MC1_READY){}
	/* Send MC2_READY byte to MC1 to ask it to send the string */
	UART_sendByte(MC2_READY);

	for (loop_count=0;loop_count<PASS_SIZE;loop_count++)
	{
		passArray_ptr[loop_count]=UART_recieveByte();
	}
}
/*
 * Description: Function to store the password in EEPROM
 * [Args] :
 *         [in]   : pointer to array where I will store password in (local array)
 */
void store_password_in_EEPROM(uint8 * passArray_ptr)
{
	uint8 loop_count; /*counter to use it in for_loop*/
	uint16 address_eeprom=address_in_eeprom;
	for (loop_count=0;loop_count<PASS_SIZE;loop_count++)
	{
		address_eeprom+=loop_count;
		/* store elements of password in EEPROM*/
		EEPROM_writeByte(address_eeprom,passArray_ptr[loop_count]); /* Write 0x0F in the external EEPROM */
		_delay_ms(20);
	}
}
/*
 * Description: Function to interfacing with micro1
 *              receive password,sent true_password message to micro1 or not
 *              then receive option_massage from micro1
 * [Args] :
 *         [in]   : pointer to array where I will store password in (local)
 */
void options(uint8 * passArray_ptr)
{
	/*receive password */
	recieve_password_using_uart(passArray_ptr);
	if (check_password(passArray_ptr))/*if password is true check will return 1(TRUE_PASSWORD)*/
	{
		UART_sendByte(TRUE_PASSWORD);/*sent to micro1 TEUE_PASSWORD */
		if(UART_recieveByte()=='-')
		{
			/*receive password */
			recieve_password_using_uart(passArray_ptr);
			/*store password in EEPROM*/
			store_password_in_EEPROM(passArray_ptr);
		}
		else
		{
			motor_on();/*function call to control motor */
		}
	}
	else
	{
		UART_sendByte(WRONG_PASSWORD);/*sent to micro1 WRONG_PASSWORD */
		recieve_password_using_uart(passArray_ptr);
		if (check_password(passArray_ptr))/*if password is true check will return 1(TRUE_PASSWORD)*/
		{
			UART_sendByte(TRUE_PASSWORD);/*sent to micro1 TEUE_PASSWORD */
			if(UART_recieveByte()=='-')
			{
				/*receive password */
				recieve_password_using_uart(passArray_ptr);
				/*store password in EEPROM*/
				store_password_in_EEPROM(passArray_ptr);
			}
			else
			{
				motor_on();/*function call to control motor */
			}
		}
		else
		{
			UART_sendByte(WRONG_PASSWORD);/*sent to micro1 WRONG_PASSWORD */
			recieve_password_using_uart(passArray_ptr);
			if (check_password(passArray_ptr))/*if password is true check will return 1(TRUE_PASSWORD)*/
			{
				UART_sendByte(TRUE_PASSWORD);/*sent to micro1 TEUE_PASSWORD */
				if(UART_recieveByte()=='-')
				{
					/*receive password */
					recieve_password_using_uart(passArray_ptr);
					/*store password in EEPROM*/
					store_password_in_EEPROM(passArray_ptr);
				}
				else
				{
					motor_on();/*function call to control motor */
				}
			}
			else
			{
				UART_sendByte(WRONG_PASSWORD);/*sent to micro1 WRONG_PASSWORD */
				/* turn on timer and buzzer for 1 minute */
				wrong_password_on();
			}
		}
	}

}
/*
 * Description: Function to COMPARE the password in EEPROM WITH NEW PASSWORD
 * [Args] :
 *         [in]   : pointer to array where I will store password in (local array)
 *         [out]   : TRUE_PASSWORD
 *                  OR  WRONG_PASSWORD
 */
uint8 check_password(uint8 * passArray_ptr)
{
	uint8 loop_count; /*counter to use it in for_loop*/
	uint16 address_eeprom=address_in_eeprom;
	uint8 key;
	for (loop_count=0;loop_count<PASS_SIZE;loop_count++)
	{
		address_eeprom+=loop_count;
		/* read elements of password in EEPROM*/
		EEPROM_readByte(address_eeprom,&key);
		_delay_ms(20);
		if (key!=passArray_ptr[loop_count])
			return WRONG_PASSWORD;
	}
	return TRUE_PASSWORD;
}
/*
 * Description:Function for
 *           1) MAKE the buzzer be ON (60 seconds)
 *              when user enter password three times wrong
 *              by set timer0 to calculate 60 seconds
 *           2) set timer call (wrong_password_off) after counting 60 seconds
 */
void wrong_password_on(void)
{
	/*initialize the BUZZER*/
	BUZZER_init();
	BUZZER_on();
	/*initialize the timer0*/
	s_timer_0_ConfigType conf={6,0,F_CPU_1024,NORMAL__MODE};
	TIMER_0_init(&conf);
	/*set call back for timer */
	TIMER_0_setCallBack(wrong_password_off);
}
/*
 * Description: Function for
 *         1) turn on motor with clock_wise for 30 second
 *            when user enter password and choose option '+'
 *            by set timer0 to calculate 33 seconds
 *         2) set timer call (motor_off) after counting 60 seconds
 */
void motor_on(void)
{
	DcMotor_Rotate(DC_MOTOR_CW);/*make motor on and with_clockwise*/
	/*initialize the timer0*/
	s_timer_0_ConfigType conf={0,250,F_CPU_1024,CTC_MODE};
	TIMER_0_init(&conf);
	/*set call back for timer */
	TIMER_0_setCallBack(motor_off);
}

