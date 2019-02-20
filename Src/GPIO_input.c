/*
 * GPIO_input.c
 *
 *  Created on: 09 Apr 2018
 *      Author: 19837739
 */

#include "GPIO_input.h"

uint32_t flowCount = 0;
uint32_t lastInterruptTime = 0;
uint8_t waterFlowFlag = 0;




void GPIO_input_init()
{

}

void flowMeterInterrupt()
{

	if(waterFlowFlag == 0)
	{
		flowCount += 100;
		lastInterruptTime = currentTime;
		waterFlowFlag = 1;
	}

}

void waterFlowFlagReset()
{
	if((waterFlowFlag == 1)  &&  (currentTime > (lastInterruptTime + DEBOUNCE_TIME)))
	{
		waterFlowFlag = 0;  // we can now take another measurement (waited DEBOUNCE_TIME ms)
	}
}

void valveControl()
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, valveState);
	//GPIOC->ODR = (GPIOC->ODR & 0xfffffffb) + (valveState << 3);
}

void heaterControlLoop()
{
	if(heatingScheduleState == 0) // Automatic heating schedule off, use last value of heaterState
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, heaterState);
	}
	else // Automatic heating schedule on
	{
		if(inWindow()) // in heating window
		{
			if(heaterTemp > (temp_setpoint + 5))
			{
				heaterState = 0;
			}
			else if(heaterTemp < (temp_setpoint - 5))
			{
				heaterState = 1;
			}
		}
		else // not in heating window, therefore turn heating off
		{
			heaterState = 0;
		}
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, heaterState);
	}

}

uint8_t inWindow()
{
	if(rtcTimeTotal >= heatingSchedule[0].Total && rtcTimeTotal < heatingSchedule[1].Total) // window 1 is valid
	{
		return 1;
	}
	else if(rtcTimeTotal >= heatingSchedule[2].Total && rtcTimeTotal < heatingSchedule[3].Total) // window 2 is valid
	{
		return 1;
	}
	else if(rtcTimeTotal >= heatingSchedule[4].Total && rtcTimeTotal < heatingSchedule[5].Total) // window 3 is valid
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

