/*
 * proxSenseI2C.h
 *
 *  Created on: 29 Apr 2018
 *      Author: Andrew
 */

#ifndef PROXSENSEI2C_H_
#define PROXSENSEI2C_H_


#include "main.h"
#include "stm32f3xx_hal.h"
#include "uart.h"
#include "adc.h"

#define TIMEOUT 2

extern I2C_HandleTypeDef hi2c1;
extern int16_t temp_setpoint; // the current temperature setpoint


void proxSenseI2CInit();

// NOT USED - using i2cIRQ() instead
// This function alternates reading the touch and coordinate data from the azoteq board
// it forces a communication window, waits 6ms for conversions to be done, then reads either touch or coord data
// it then closes the communication window and allows 2ms for the azoteq board to run routines before forcing a new communication window.
void i2cLoop();

// This function alternates reading the touch and coordinate data from the azoteq board
// The function is called on an event interrupt from the azoteq board
// The event interrupt occurs when a touch event is detected by the azoteq board
void i2cIRQ();

void printSettings();

int scanAdresses();





#endif /* PROXSENSEI2C_H_ */
