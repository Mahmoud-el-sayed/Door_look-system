/******************************************************************************
 *
 * Module: timer0
 *
 * File Name: timer0.h
 *
 * Description: Header file for the timer0 driver
 *
 * Author: mahmoud Mohamed
 *
 *******************************************************************************/
#ifndef TIMER0_H_
#define TIMER0_H_
/*******************************************************************************
 *                                includes                                 *
 *******************************************************************************/
#include"std_types.h"
/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/


/*******************************************************************************
 *  Enum name : e_timer_0_Clock
 *  Enum Description:
 *  this enum is responsible for the clock select
 *  where: bit 2:0 from TCCRO Register (cs02:0)
 *  NO_CLOCK     = (value 0) : No Clock source and timer/counter stopped
 *  F_CPU_CLOCK  = (value 1) : clock /(no prescaling)
 *  F_CPU_8      = (value 2) : clock/8 (from prescaler)
 *  F_CPU_64     = (value 3) : clock/64 (from prescaler)
 *  F_CPU_256    = (value 4) : clock/256 (from prescaler)
 *  F_CPU_1024   = (value 5) : clock/1024 (from prescaler)
 *  EXTERNAL_TO_PIN_FALLING_EDGE = (value 6) :External clock source on T0 pin , clock on falling edge
 *  EXTERNAL_TO_PIN_RISING_EDGE  = (value 7) :External clock source on T0 pin , clock on rising edge
 */
typedef enum
{
	NO_CLOCK,F_CPU_CLOCK,F_CPU_8,F_CPU_64,F_CPU_256,F_CPU_1024,
	EXTERNAL_TO_PIN_FALLING_EDGE,EXTERNAL_TO_PIN_RISING_EDGE
}e_timer_0_Clock;

/*******************************************************************************
 *  Enum name : e_timer_0_mode
 *  Enum Description:
 *  this enum is responsible for the Mode select of timer_0  or
 *  where: bit 3,6 from TCCRO Register  (WGM01:0)
 *  NORMAL_MODE     = (value 0) : normal mode (top=0xFF)
 *  CTC_MODE        = (value 2) : compare mode(top=OCR0)
 */
typedef enum
{
	NORMAL__MODE,CTC_MODE=2
}e_timer_0_mode;

/*******************************************************************************
 *  Structure name : s_timer_0_ConfigType
 *  Structure Description:
 *  this Structure is responsible for
 *  1-the Mode select
 *  2-Initial value
 *  3-clock select
 *  4-compare value in case compare mode
 */
typedef struct
{
	uint8 initial_value;
	uint8 compare_value;
	e_timer_0_Clock clock;
	e_timer_0_mode mode;
}s_timer_0_ConfigType;
/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description: Function to set the Call Back function address.
 */
void TIMER_0_setCallBack(void(*a_ptr)(void));
 /*
 *  Description : Function to initialize the timer0
 * 	1. Set the required clock.
 * 	2. Set the required initial value of timer0.
 * 	3. set the required mode
 * 	4. set compare value of timer0 in case CTC
 * 	5-NON_PWM_MODE FOCO=1
 */
void TIMER_0_init(const s_timer_0_ConfigType * Config_Ptr);
/*
 * Description: Function to disable the Timer0
 * 1-NO_CLK
 * 2-Disenable interrupt mask register
 */
void TIMER_0_deInit(void);
#endif /* TIMER0_H_ */
