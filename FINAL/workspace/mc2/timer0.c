/******************************************************************************
 *
 * Module: timer0
 *
 * File Name: timer0.c
 *
 * Description: source file for the timer0 driver
 *
 * Author: mahmoud Mohamed
 *
 *******************************************************************************/

/*******************************************************************************
 *                                includes                                 *
 *******************************************************************************/
#include"timer0.h"
#include <avr/io.h> /* To use /Timer0 Registers */
#include <avr/interrupt.h>
/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/
/* Global variables to hold the address of the call back function in the application */
static volatile void (*g_callBackPtr)(void) = NULL_PTR;
/* Global variable to hold the intial value of timer0 in case over flow
 * and start!=0
 *  */
static uint8 g_intialValue_timer_0;
/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/
ISR(TIMER0_OVF_vect)
{
	if(g_callBackPtr != NULL_PTR)
		{
			/* Call the Call Back function in the application after the timer finish */
			(*g_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
		}

	/* Set the required initial value of timer0
		 * because it will return to zero
		 * */
		TCNT0=g_intialValue_timer_0;

}
ISR(TIMER0_COMP_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the timer finish */
		(*g_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}
/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description: Function to set the Call Back function address.
 */
void TIMER_0_setCallBack(void(*a_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_callBackPtr = a_ptr;

}

/*
 *  Description : Function to initialize the timer0
 * 	1. Set the required clock.
 * 	2. Set the required initial value of timer0.
 * 	3. set the required mode
 * 	4. set compare value of timer0 in case CTC
 * 	5-NON_PWM_MODE FOC0=1
 */
void TIMER_0_init(const s_timer_0_ConfigType * Config_Ptr)
{
	/* MAKE SURE That TCCR0 =0 */
	TCCR0=0;

	/* 1-NON_PWM_MODE FOC0=1
	 * 2- bit 1 of mode_variable shift left by 2 times to set in bit 3 (WGM010=0:(NORMAL)/WGM010=1:(CTC)) of TCCR0
	 * 3- bit 6 (WGM00) of register TTCR0 =0 in two modes
	 * 4- bit 0:2 (CS00:CS02) are set by value in clock_variable
	 * 5- bit 5:4 (COM01:00) are cleared (OCO disconnected)
	 * 6- enable interrupt mask
	 */
	TCCR0=(1<<FOC0)|((Config_Ptr->mode)<<2)|(Config_Ptr->clock);

	/* Set the required initial value of timer0.*/
	TCNT0=Config_Ptr->initial_value;

	/* Global variable to hold the intial value of timer0 in case over flow
	 * and start!=0
	 *  */
	g_intialValue_timer_0=Config_Ptr->initial_value;

	/* Set the required compare mode variable  of timer0 in case CTC only */
	OCR0=Config_Ptr->compare_value;

	/*enable interrupt mask register in two cases
	 * OCIE=1 (CTC) or TOIE0=1 (OVO)
	*/
	if((Config_Ptr->mode)==NORMAL__MODE)
	    TIMSK|= (1<<TOIE0);
	else
		TIMSK |= (1<<OCIE0);
}
/*
 * Description: Function to disable the Timer0
 * 1-NO_CLK
 * 2-DisEnable interrupt mask register
 */
void TIMER_0_deInit(void)
{
	TCCR0=0; /* no clock and set register as intial value*/
	TIMSK&=0xfc; /* clear bit 1 and bit 0 */
}

