/******************************************************************************
 *
 * Module: main
 *
 * File Name: mc1.c
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
#include"keypad.h"
#include"lcd.h"
#include"uart.h"
#include"std_types.h"
/*******************************************************************************
 *                                macros                                   *
 *******************************************************************************/
#define ERROR                            1  /*refer to  password less than size or grater*/
#define DONE                             0  /* password is done */
#define MC1_READY                        0x20
#define MC2_READY                        0x10
#define PASS_SIZE                        5  /*refer to size of password*/
#define WRONG_PASSWORD                   0
#define TRUE_PASSWORD                    1
/*for one second :need 64 overflow (interrupt)*/
/* why 32 : (1024/(8*10^6))*250*(10^3)
 * where  1024   :  prescaler
 *        8*10^6 :  F_cpu
 *        10^3   :  one second
 *        250    :  number of interrupts
 */

#define NUMBER_OF_OVERFLOWS_for_1_SECOND 32
/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description: Function for initialize  all drivers
 *  1)Enable global interrupts
 *  2)LCD
 *  3)UART
 */
void init_microcontroller(void);
/*
 * Description: Function to take the password from user at beginning of system in two times
 * [Args] :
 *         [in]   : pointer to array where I will store password in (global array)
 */
void set_password(uint8 * passArray_ptr);
/*
 * Description: Function to take the password from user at beginning of system (first time)
 * [Args] :
 *         [in]   : pointer to array where I will store password in (global array)
 *         [out]  :    DONE  : mean that the user enter password with same PASS_SIZE
 *                             and pressed enter after password
 *                  or Error : reverse of case Done
 */
uint8 take_password_at_first_time(uint8 * passArray_ptr);
/*
 * Description: Function to take the password from user at beginning of system (second time)
 * [Args] :
 *         [in]   : pointer to array where I will store password in (global array)
 *         [out]  :    DONE  : mean that the user enter password matching with first time
 *                  or Error : reverse of case Done
 */
uint8 confirm_password_at_first(uint8 * passArray_ptr);
/*
 * Description: Function to sent the password to microcontroller2 using UART
 * [Args] :
 *         [in]   : pointer to array where I will store password in (global array)
 */
void sent_password_using_uart(uint8 * passArray_ptr);
/*
 * Description: Function to show options on LCD
 * and take the option from user
 * [Args] :
 *         [out]   : '+' or '-'
 */
uint8 show_options(void);
/*
 * Description: Function to take password from user and compare it with in eeprom
 * [Args] :
 *         [in]   : pointer to array where I will store password in (global array)
 */
void enter_pass(uint8 * passArray_ptr);
/*
 * Description: Function to interfacing with microcontroller2
 *                          if password is true : open door or change password
 *                                          else: wait 1 minute
 * [Args] :
 *         [in]   : pointer to array where I will store password in (global array)
 */
void options(uint8 * passArray_ptr);
/*
 * Description:Function for
 *           1) waiting till the buzzer be off (60 seconds)
 *              when user enter password three times wrong
 *              by set timer0 to calculate 60 seconds
 *           2) show error massage on LCD for 60 seconds
 *           3) set timer call (wrong_password_off) after counting 60 seconds
 */
void wrong_password_on(void);
/*
 * Description: Function for waiting till the motor be off (33 seconds)
 * when user enter password and choose option '+'
 */
void waiting_motor_on(void);
/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/
/* array of size PASS_SIZE elements to hold password */
uint8 pass_array[PASS_SIZE];
/* Global variable to counting times of run Ultrasonic_edgeProcessing function
 *  */
uint16 g_tick=0;
/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description: Function for call back for timer0
 * to deinit timer0 and clear lcd from error message
 */
void wrong_password_off(void)
{
	g_tick++; /*if NUMBER_OF_OVERFLOWS_for_1_SECOND occur so counter will added by 1 */
	if (g_tick==NUMBER_OF_OVERFLOWS_for_1_SECOND*60)
	{
		g_tick=0; /*clear counter to count from 0 again*/
		TIMER_0_deInit(); /*turn off timer*/
	}
}
/*
 * Description: Function for call back for timer0
 * to deinit timer0
 * and show case of motor (open ,stop,close)
 */
void waiting_motor_off(void)
{
	g_tick++; /*if NUMBER_OF_OVERFLOWS_for_1_SECOND occur so counter will added by 1 */
	if (g_tick==NUMBER_OF_OVERFLOWS_for_1_SECOND*15)
	{
		/*clear LCD */
		LCD_clearScreen();
		/*use LCD to print message : "door is stop" */
		LCD_displayString("Door is stop");
	}
	if (g_tick==NUMBER_OF_OVERFLOWS_for_1_SECOND*18)
	{
		/*clear LCD */
		LCD_clearScreen();
		/*use LCD to print message : "door is closing" */
		LCD_displayString("Door is closing");

	}
	if (g_tick==NUMBER_OF_OVERFLOWS_for_1_SECOND*33)
	{
		TIMER_0_deInit();/*turn off timer*/
		g_tick=0;/*clear counter to count from 0 again*/

	}
}
/*
 * Description: main Function
 *  1)initialize  all drivers
 *  2)set password at first time
 *  3)sent password to microcontroller2 using UART
 *  4)show options in while(1)
 */
int main(void)
{
	/*initialize  all drivers */
	init_microcontroller();

	/*set password at first time */
	set_password(pass_array);
	/* sent password to microcontroller2*/
	sent_password_using_uart(pass_array);
	while(1)
	{
		/* show  main options*/
		options(pass_array);
	}
}
/*
 * Description: Function for initialize  all drivers
 *  1)Enable global interrupts
 *  2)LCD
 *  3)UART
 */
void init_microcontroller(void)
{
	/*Enable global interrupts in MC by setting the I-Bit.*/
	SREG |= (1<<7);
	/*initialize the LCD*/
	LCD_init();
	/*clear LCD */
	LCD_clearScreen();
	/*initialize the UART*/
	s_uart_ConfigType conf_1={_8_BITS_SIZE,DISABLED_PARITY,_1_BIT_STOP,9600};
	UART_init(&conf_1);
}
/*
 * Description: Function to take the password from user at beginning of system in two times
 * [Args] :
 *         [in]   : pointer to array where I will store password in (global array)
 */
void set_password(uint8 * passArray_ptr)
{
	do
	{
		/* if user enter password matching with size function will return 0 (false for while)
		 * else return 1 (true for while)
		 */
		while(take_password_at_first_time(passArray_ptr)){}
	} while (confirm_password_at_first(passArray_ptr));
	/* function (confirm_password) return 1 in case not matching  or 0 in case  matching */
}
/*
 * Description: Function to take the password from user at beginning of system (first time)
 * [Args] :
 *         [in]   : pointer to array where I will store password in (global array)
 *         [out]  :    DONE  : mean that the user enter password with same PASS_SIZE
 *                             and pressed enter after password
 *                  or Error : reverse of case Done
 */
uint8 take_password_at_first_time(uint8 * passArray_ptr)
{
	uint8 loop_count=0; /*counter to use it in for_loop*/
	uint8 key; /*to hold the value return of keypad*/
	/*clear LCD */
	LCD_clearScreen();
	/*use LCD to print message : "enter pass" */
	LCD_displayString("Enter N_Password");
	/* move the cursor of LCD to print the pass as ***** */
	LCD_moveCursor(1,0);
	/* for_loop counts from 0 to size of password
	 * where: from (loop_count=0)   to (loop_count=PASS_SIZE-1) take password from user
	 *        final COUNT (loop_count=PASS_SIZE) for take (>>enter<< (ON/C)) from user
	 */
	for (;loop_count<=(PASS_SIZE);loop_count++)
	{
		/* Press time for keypad */
		_delay_ms(500);
		/* take elements of password using Keypad */
		key=KEYPAD_getPressedKey();

		/*make sure that the user  enter password as size
		 * 13 : ASCII of >>enter<< (ON/C)
		 */
		if (((loop_count<PASS_SIZE)&&(13==key))||((loop_count==PASS_SIZE)&&(13!=key)))
			return ERROR;
		/* store the element in array and show '*'
		 * but first doesn't include  >>enter<< (ON/C) in password
		 */
		if ((loop_count!=PASS_SIZE))
		{
			LCD_displayCharacter('*'); /* display * for every digit enter*/
			*(passArray_ptr+loop_count)=key;  /* store digits in array*/
		}
	}
	return DONE ;
}
/*
 * Description: Function to take the password from user at beginning of system (second time)
 * [Args] :
 *         [in]   : pointer to array where I will store password in (global array)
 *         [out]  :    DONE  : mean that the user enter password matching with first time
 *                  or Error : reverse of case Done
 */
uint8 confirm_password_at_first(uint8 * passArray_ptr)
{
	uint8 loop_count=-1; /*counter to use it in for_loop*/
	uint8 key; /*to hold the value return of keypad*/
	uint8 wrong=0; /*as flag will equal 1 if element at least wrong*/

	/*clear LCD */
	LCD_clearScreen();
	/*use LCD to print message : "enter pass" */
	LCD_displayString("RE-Enter N_Pass");
	/* move the cursor of LCD to print the pass as ***** */
	LCD_moveCursor(1,0);
	do
	{
		loop_count++;
		/* Press time for keypad */
		_delay_ms(500);
		/* take elements of password using Keypad */
		key=KEYPAD_getPressedKey();
		/* wrong will be set if user enter password's length not equal password's length at first time */
		if (((loop_count<PASS_SIZE)&&(13==key))||((loop_count<PASS_SIZE)&&(*(passArray_ptr+loop_count)!=key)))
		{
			wrong=1;
		}
		if (key!=13)
		{
			LCD_displayCharacter('*'); /* display * for every digit enter*/
		}
	} while ((key!=13)&&(loop_count<16));
	/* not matched in three cases
	 * 1)second time 's length is less than at first
	 * 2)second time 's length is large than at first
	 * 3)second time not matched with first
	 * */
	if ((loop_count>PASS_SIZE)||(wrong==1))
		return ERROR  ;
	return DONE  ;
}
/*
 * Description: Function to sent the password to microcontroller2 using UART
 * [Args] :
 *         [in]   : pointer to array where I will store password in (global array)
 */
void sent_password_using_uart(uint8 * passArray_ptr)
{
	uint8 loop_count; /*counter to use it in for_loop*/

	/*you can delete this check */

	/*for checking that two microcontrollers working in same time */
	UART_sendByte(MC1_READY);
	while(UART_recieveByte() != MC2_READY){}

	/* sent password to  store it in EEPROM*/
	for (loop_count=0;loop_count<PASS_SIZE;loop_count++)
	{
		/* Send the required string to MC2 through UART */
		UART_sendByte(passArray_ptr[loop_count]);
	}
}
/*
 * Description: Function to show options on LCD
 * and take the option from user
 * [Args] :
 *         [out]   : '+' or '-'
 */
uint8 show_options(void)
{
	uint8 option ; /*TO hold + or - */
	do
	{
		/*clear LCD */
		LCD_clearScreen();
		/*use LCD to print message : "enter pass" */
		LCD_displayString("+: open door  ");
		/* move the cursor of LCD to print the pass as ***** */
		LCD_moveCursor(1,0);
		LCD_displayString("-:change pass ");
		/* take elements of password using Keypad */
		option=KEYPAD_getPressedKey();
	} while ((option!='+')&&(option!='-'));
	return option;
}
/*
 * Description: Function to take password from user and compare it with in eeprom
 * [Args] :
 *         [in]   : pointer to array where I will store password in (global array)
 */
void enter_pass(uint8 * passArray_ptr)
{
	uint8 loop_count=-1; /*counter to use it in for_loop*/
	uint8 key; /*to hold the value return of keypad*/

	/*clear LCD */
	LCD_clearScreen();
	/*use LCD to print message : "enter pass" */
	LCD_displayString("Enter Password:");
	/* move the cursor of LCD to print the pass as ***** */
	LCD_moveCursor(1,0);

	do
	{
		loop_count++;
		_delay_ms(500); /* Press time for keypad */
		/* take elements of password using Keypad */
		key=KEYPAD_getPressedKey();

		if ((key!=13)&&(loop_count<PASS_SIZE))
		{
			LCD_displayCharacter('*'); /* display * for every digit enter*/
			*(passArray_ptr+loop_count)=key;
		}
		else if ((loop_count>=PASS_SIZE)&&(13!=key))
		{
			LCD_displayCharacter('*');/* display * for every digit enter*/
			*(passArray_ptr)=13;
			/* 13 :ASCCI code for enter key*/
			/*if user enter password's length > saved password's length
			 * store first digit by enter_key because it possible
			 * so in comparing the result will be error
			 */
		}
		else if ((loop_count<PASS_SIZE)&&(13==key))
		{
			*(passArray_ptr)=13;
			/* 13 :ASCCI code for enter key*/
			/*if user enter password's length < saved password's length
			 * store first digit by enter_key because it possible
			 * so in comparing the result will be error
			 */
		}

	} while ((key!=13)&&(loop_count<16));
}
/*
 * Description: Function to interfacing with microcontroller2
 *                          if password is true : open door or change password
 *                                          else: wait 1 minute
 * [Args] :
 *         [in]   : pointer to array where I will store password in (global array)
 */
void options(uint8 * passArray_ptr)
{
	uint8 option; /* hold + or - */
	/*show + or - in LCD and options */
	option=show_options();
	/*enter password to make option you choose*/
	enter_pass(passArray_ptr);
	/*sent it to microcontroller2 to check it*/
	sent_password_using_uart(passArray_ptr);

	if (UART_recieveByte()==TRUE_PASSWORD)
	{
		UART_sendByte(option);
		if(option=='-')
		{
			/*set password at first time */
			set_password(pass_array);
			sent_password_using_uart(pass_array);
		}
		else
		{
			waiting_motor_on();
			while(g_tick!=1055){}/*delay for 33 seconds*/
			/*where 1055 is (((33(second))*(32(NUMBER_OF_OVERFLOWS_for_1_SECOND))-1)*/
		}
	}
	else
	{
		/*enter password to make option you choose*/
		enter_pass(passArray_ptr);
		sent_password_using_uart(passArray_ptr);
		if (UART_recieveByte()==TRUE_PASSWORD)
		{
			UART_sendByte(option);
			if(option=='-')
			{
				/*set password at first time */
				set_password(pass_array);
				sent_password_using_uart(pass_array);
			}
			else
			{
				waiting_motor_on();
				while(g_tick!=1055){}/*delay for 33 seconds*/
				/*where 1055 is (((33(second))*(32(NUMBER_OF_OVERFLOWS_for_1_SECOND))-1)*/
			}
		}
		else
		{
			/*enter password to make option you choose*/
			enter_pass(passArray_ptr);
			sent_password_using_uart(passArray_ptr);
			if (UART_recieveByte()==TRUE_PASSWORD)
			{
				UART_sendByte(option);
				if(option=='-')
				{
					/*set password at first time */
					set_password(pass_array);
					sent_password_using_uart(pass_array);
				}
				else
				{
					waiting_motor_on();
					while(g_tick!=1055){}/*delay for 33 seconds*/
					/*where 1055 is (((33(second))*(32(NUMBER_OF_OVERFLOWS_for_1_SECOND))-1)*/
				}
			}
			else
			{
				wrong_password_on();
				while(g_tick!=1919){}/*delay for 60 seconds*/
				/*where 1055 is (((60(second))*(32(NUMBER_OF_OVERFLOWS_for_1_SECOND))-1)*/
				/*clear LCD */
				LCD_clearScreen();
			}
		}
	}
}
/*
 * Description:Function for
 *           1) waiting till the buzzer be off (60 seconds)
 *              when user enter password three times wrong
 *              by set timer0 to calculate 60 seconds
 *           2) show error massage on LCD for 60 seconds
 *           3) set timer call (wrong_password_off) after counting 60 seconds
 */
void wrong_password_on(void)
{
	/*clear LCD */
	LCD_clearScreen();
	/*use LCD to print message : "enter pass" */
	LCD_displayString("Error");
	/*initialize the timer0*/
	s_timer_0_ConfigType conf={6,0,F_CPU_1024,NORMAL__MODE};
	TIMER_0_init(&conf);
	/*set call back for timer */
	TIMER_0_setCallBack(wrong_password_off);
}
/*
 * Description: Function for
 *         1) waiting till the motor be off (33 seconds)
 *            when user enter password and choose option '+'
 *            by set timer0 to calculate 33 seconds
 *         2) set timer call (waiting_motor_off) after counting 60 seconds
 *         3)show on LCD "Door is opening"
 */
void waiting_motor_on(void)
{
	/*clear LCD */
	LCD_clearScreen();
	/*use LCD to print message : "door is opening" */
	LCD_displayString("Door is opening");
	/*initialize the timer0*/
	s_timer_0_ConfigType conf={0,250,F_CPU_1024,CTC_MODE};
	TIMER_0_init(&conf);
	/*set call back for timer */
	TIMER_0_setCallBack(waiting_motor_off);
}
