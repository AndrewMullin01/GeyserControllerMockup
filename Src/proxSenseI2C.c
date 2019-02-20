/*
 * proxSenseI2C.c
 *
 *  Created on: 29 Apr 2018
 *      Author: Andrew
 */
#include "proxSenseI2C.h"

uint16_t sliderAddr = 0x44 << 1;
uint16_t rotaryAddr = 0x45 << 1;
uint8_t delayFlag = 0;

uint16_t settingsBytes =  0x09;
uint16_t coordBytes = 0x02;
uint16_t touchBytes = 0x03;
uint8_t slideCBuff;
uint8_t sliderBusyFlag = 0; // indicates when user is adjusting temperature set point
uint8_t rotatoCBuff;
uint8_t settingsBuff[10];
uint8_t settings[5] = {0x80, 0x59, 0x00, 0x00, 0xff};
uint8_t comStatus = 0;
uint8_t addressFlag = 0;



void proxSenseI2CInit()
{
	//scanAdresses();
	//printSettings();

	// set board to slider/wheel mode
	do
	{
		//printf("Writing slider settings...\r\n");
		comStatus = HAL_I2C_Mem_Write(&hi2c1, sliderAddr, settingsBytes, I2C_MEMADD_SIZE_8BIT, settings, 5, TIMEOUT);

	}
	while(comStatus != HAL_OK);
	//printf("Succez. %d\r\n", comStatus);
	comStatus = 0;
	do
	{
		//printf("Writing rotato settings...\r\n");
		comStatus =HAL_I2C_Mem_Write(&hi2c1, rotaryAddr, settingsBytes, I2C_MEMADD_SIZE_8BIT, settings, 5, TIMEOUT);
	}
	while(comStatus != HAL_OK);
	//printf("Succez x2. %d\r\n", comStatus);
	//printSettings();


}

void i2cLoop()
{
	if(delayFlag == 0)
	{
		// force communication window
		//////HAL_GPIO_WritePin(GPIOA, SliderRDY_Pin, 0);
		// go to the next state
		delayFlag++;

	}
	else if(delayFlag == 3 && addressFlag == 0) //  6ms has passed
	{
		HAL_I2C_Mem_Read(&hi2c1, sliderAddr, coordBytes, I2C_MEMADD_SIZE_8BIT, &slideCBuff, (uint16_t)1, TIMEOUT);

		// close communication window
		//////HAL_GPIO_WritePin(GPIOA, SliderRDY_Pin, 1);

		// go to the next state
		addressFlag = 1;
		delayFlag++;
	}
	else if(delayFlag == 3 && addressFlag == 1) //  or 6ms has passed
	{
		HAL_I2C_Mem_Read(&hi2c1, sliderAddr, touchBytes, I2C_MEMADD_SIZE_8BIT, &sliderBusyFlag, (uint16_t)1, TIMEOUT);

		if(sliderBusyFlag > 1)
		{
			temp_setpoint = slideCBuff;
		}
		// close communication window
		//////HAL_GPIO_WritePin(GPIOA, SliderRDY_Pin, 1);

		// go to the next state
		addressFlag = 0;
		delayFlag++;
	}
	else if(delayFlag >= 4)
	{
		// allow 2ms for azoteq routines
		// return to first state
		delayFlag = 0;
	}

	else
	{
		// go to the next state
		delayFlag++;
	}

}

void i2cIRQ()
{
	if(addressFlag == 0)
	{
		HAL_I2C_Mem_Read(&hi2c1, sliderAddr, coordBytes, I2C_MEMADD_SIZE_8BIT, &slideCBuff, (uint16_t)1, TIMEOUT);
	}
	else if(addressFlag == 3)
	{
		HAL_I2C_Mem_Read(&hi2c1, sliderAddr, touchBytes, I2C_MEMADD_SIZE_8BIT, &sliderBusyFlag, (uint16_t)1, TIMEOUT);

		if(sliderBusyFlag > 1)
		{
			temp_setpoint = (uint16_t)map(slideCBuff, 1, 125, 0, 100);
		}
	}

	if(++addressFlag == 6) addressFlag = 0;

}


void printSettings()
{

	HAL_I2C_Mem_Read(&hi2c1, sliderAddr, settingsBytes, I2C_MEMADD_SIZE_8BIT, settingsBuff, (uint16_t)5 , 10);
	printf("Slider settings: %d %d %d %d %d\r\n", settingsBuff[0], settingsBuff[1], settingsBuff[2], settingsBuff[3], settingsBuff[4]);
	HAL_I2C_Mem_Read(&hi2c1, rotaryAddr, settingsBytes, I2C_MEMADD_SIZE_8BIT, &settingsBuff[5], (uint16_t)5 , 10);
	printf("Rotato settings: %d %d %d %d %d\r\n", settingsBuff[5], settingsBuff[6], settingsBuff[7], settingsBuff[8], settingsBuff[9]);

}


// taken from https://github.com/ProjectsByJRP/stm32_hal_i2c_bus_scan
int scanAdresses()
{
	printf("Scanning I2C bus:\r\n");
	HAL_StatusTypeDef result;
	uint8_t i;
	uint8_t j = 0;
	for (i=1; i<128; i++)
	{
		/*
		 * the HAL wants a left aligned i2c address
		 * &hi2c1 is the handle
		 * (uint16_t)(i<<1) is the i2c address left aligned
		 * retries 2
		 * timeout 2
		 */
		result = HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(i<<1), 2, 2);
		if (result != HAL_OK) // HAL_ERROR or HAL_BUSY or HAL_TIMEOUT
		{
			printf("."); // No ACK received at that address
		}
		if (result == HAL_OK)
		{
			printf("0x%X", i); // Received an ACK at that address
			j++;
		}
	}
	printf("\r\n");

	return j;
}
