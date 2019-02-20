/*
 * 7Seg.h
 *
 *  Created on: 08 Mar 2018
 *      Author: 19837739
 */

#ifndef SEVSEG_H_
#define SEVSEG_H_

#include "main.h"
#include "stm32f3xx_hal.h"
#include "proxSenseI2C.h"

// extern variable declarations - tell the compiler that these variables exist, and are defined elsewhere
extern int16_t temp_setpoint; // the current temperature setpoint
extern int it_count;
extern uint8_t sliderBusyFlag; // indicates when user is adjusting temperature set point
extern uint16_t heaterTemp;

// function prototypes
void SevSegInit (void);

void displayLoop();

void digitLoop(int16_t num);

void SevSegwrite(uint8_t num);

void SevSegReset();



#endif /* SEVSEG_H_ */
