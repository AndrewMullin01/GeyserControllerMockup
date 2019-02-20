/*
 * 7Seg.c
 *
 *  Created on: 08 Mar 2018
 *      Author: 19837739
 */

#include "SevSeg.h"

uint8_t digitCount;
char tempratureString[3];

void SevSegInit()
{
	digitCount = 0;

	GPIOA->ODR = (GPIOA->ODR | 0x000003e0);
	GPIOB->ODR = (GPIOB->ODR | 0x00000478);
	GPIOC->ODR = (GPIOC->ODR | 0x00000080);
}

// resets seven segment bits high
void SevSegReset()
{
	GPIOA->ODR = (GPIOA->ODR | 0x000003e0);
	GPIOB->ODR = (GPIOB->ODR | 0x00000040);
	GPIOC->ODR = (GPIOC->ODR | 0x00000080);
}

void displayLoop()
{
	if(sliderBusyFlag <= 1) // normal operation
	{
		digitLoop(heaterTemp);
	}
	else // user is adjusting tempSetPoint slider
	{
		digitLoop(temp_setpoint);
	}
}

void digitLoop(int16_t num)
{

	GPIOB->ODR = (GPIOB->ODR | 0x00000478); //set all digits off

	switch (digitCount)
	{
	case 0:
		GPIOB->ODR = (GPIOB->ODR & 0xfffffbff);

		SevSegwrite((uint8_t)(num/1000));
		break;

	case 1:
		GPIOB->ODR = (GPIOB->ODR & 0xffffffef);
		SevSegwrite((uint8_t)((num%1000)/100));
		break;

	case 2:
		GPIOB->ODR = (GPIOB->ODR & 0xffffffdf);
		SevSegwrite((uint8_t)((num%100)/10));
		break;

	case 3:
		GPIOB->ODR = (GPIOB->ODR & 0xfffffff7);
		SevSegwrite((uint8_t)(num%10));
		break;
	}

	if(++digitCount == 4) digitCount = 0;



}

void SevSegwrite(uint8_t num)
{
	SevSegReset();

	switch (num)
	{
	case 0 :

		GPIOA->ODR = (GPIOA->ODR & 0xfffffe1f);
		GPIOB->ODR = (GPIOB->ODR & 0xffffffbf);
		GPIOC->ODR = (GPIOC->ODR & 0xffffff7f);
		break;

	case 1:
		GPIOA->ODR = (GPIOA->ODR & 0xffffff3f);
		break;

	case 2:
		GPIOA->ODR = (GPIOA->ODR & 0xfffffd9f);
		GPIOB->ODR = (GPIOB->ODR & 0xffffffbf);
		GPIOC->ODR = (GPIOC->ODR & 0xffffff7f);
		break;

	case 3:
		GPIOA->ODR = (GPIOA->ODR & 0xfffffd1f);
		GPIOB->ODR = (GPIOB->ODR & 0xffffffbf);
		break;

	case 4:
		GPIOA->ODR = (GPIOA->ODR & 0xfffffc3f);
		break;

	case 5:
		GPIOA->ODR = (GPIOA->ODR & 0xfffffc5f);
		GPIOB->ODR = (GPIOB->ODR & 0xffffffbf);
		break;

	case 6:
		GPIOA->ODR = (GPIOA->ODR & 0xfffffc5f);
		GPIOB->ODR = (GPIOB->ODR & 0xffffffbf);
		GPIOC->ODR = (GPIOC->ODR & 0xffffff7f);
		break;

	case 7:
		GPIOA->ODR = (GPIOA->ODR & 0xffffff1f);
		break;

	case 8:
		GPIOA->ODR = (GPIOA->ODR & 0xfffffc1f);
		GPIOB->ODR = (GPIOB->ODR & 0xffffffbf);
		GPIOC->ODR = (GPIOC->ODR & 0xffffff7f);
		break;

	case 9:
		GPIOA->ODR = (GPIOA->ODR & 0xfffffc1f);
		break;

	}
}

