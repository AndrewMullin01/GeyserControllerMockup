/*
 * uart.h
 *
 *  Created on: 08 Mar 2018
 *      Author: 19837739
 */

#ifndef UART_H_
#define UART_H_

#include <stdbool.h>
#include "main.h"
#include "stm32f3xx_hal.h"
#include "GPIO_input.h"
#include "adc.h"

#define CMDBUFLEN 50   // maximum length of a command string received on the UART
#define MAXTXLEN  50   // maximum length of transmit buffer (replies sent back to UART)

typedef struct TIME_Structure{
  uint8_t Hours;
  uint8_t Minutes;
  uint8_t Seconds;
  uint32_t Total;
}timeStruct;

// extern variable declarations - tell the compiler that these variables exist, and are defined elsewhere
extern UART_HandleTypeDef huart1;
extern RTC_HandleTypeDef hrtc;
extern int16_t temp_setpoint; // the current temperature setpoint
extern int fastCurrentRMS;
extern int fastVoltageRMS;
extern int currentRMS;
extern int voltageRMS;
extern int16_t valveState;
extern int16_t heaterState;
extern int16_t heatingScheduleState;
extern uint16_t ambientTemp;
extern uint16_t heaterTemp;
extern uint32_t flowCount;
extern timeStruct heatingSchedule[6];
extern uint32_t rtcTimeTotal;
extern int16_t  loggingState;

// function prototypes

void DecodeCommand(void);

void uartInit(void);

// UserMainLoopUpdate - function that gets called from main, with our user code in it. This function does not in itself contain a loop, but is called with every iteration of the main loop.
void uartLoopCheck(void);


uint8_t Int2String(char* output_string, int32_t val, uint8_t maxlen);

uint8_t String2Int(char* input_string, int16_t* output_integer);

uint8_t String2IntU8(char* input_string, uint8_t* output_integer);

//////////////////////////////////////////////////////////////////////
// to implement printf()
//////////////////////////////////////////////////////////////////////
int __io_putchar(int ch);
int _write(int file,char *ptr, int len);


#endif /* UART_H_ */
