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
#include"buzzer.h"



/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*

/*
 *  Description : Function to initialize the buzzer
 * 	with Set direction of the pin
 */
void BUZZER_init(void)
{
	/*
	 * Set pin as output for buzzer
	 * */
	GPIO_setupPinDirection(BUZZER_PORT, BUZZER_PIN, PIN_OUTPUT);
	GPIO_writePin(BUZZER_PORT, BUZZER_PIN, OFF);
}

/*
 *  Description : Function to on the buzzer
 * 	1. Set the pin
 */
void BUZZER_on(void)
{
	/* turn on buzzer*/
	GPIO_writePin(BUZZER_PORT, BUZZER_PIN, ON);
}


/*
 *  Description : Function off the buzzer
 */
void BUZZER_off(void)
{
	/* turn off buzzer*/
	GPIO_writePin(BUZZER_PORT, BUZZER_PIN, OFF);
}
