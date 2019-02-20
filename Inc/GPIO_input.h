/*
 * GPIO_input.h
 *
 *  Created on: 09 Apr 2018
 *      Author: 19837739
 */

#ifndef GPIO_INPUT_H_
#define GPIO_INPUT_H_

#include "main.h"
#include "stm32f3xx_hal.h"
#include "uart.h"


#define DEBOUNCE_TIME 6

extern uint32_t currentTime;
extern int16_t valveState;
extern int16_t heaterState;
extern uint16_t heaterTemp;
extern int16_t temp_setpoint;
extern int16_t heatingScheduleState;
extern RTC_TimeTypeDef rtcTime;





void GPIO_input_init();

void flowMeterInterrupt();

void waterFlowFlagReset();

void valveControl();

void heaterControlLoop();

uint8_t inWindow();






#endif /* GPIO_INPUT_H_ */
