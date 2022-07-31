/******************************************************************************
 *
 * Module: timer0
 *
 * File Name: buzzer.h
 *
 * Description: Header file for the timer0 driver
 *
 * Author: mahmoud Mohamed
 *
 *******************************************************************************/

#ifndef BUZZER_H_
#define BUZZER_H_
/*******************************************************************************
 *                                includes                                 *
 *******************************************************************************/
#include"gpio.h"

/*******************************************************************************
 *                                 macros                                   *
 *******************************************************************************/
/* buzzer HW Port and Pin Id */
#define BUZZER_PORT PORTC_ID
#define BUZZER_PIN  PIN7_ID
/* define on and off */
#define ON 1
#define OFF 0

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 *  Description : Function to initialize the buzzer
 * 	with Set direction of the pin
 */
void BUZZER_init(void);

/*
 *  Description : Function to on the buzzer
 * 	1. Set the pin
 */
void BUZZER_on(void);


/*
 *  Description : Function off the buzzer
 */
void BUZZER_off(void);


#endif /* BUZZER_H_ */
