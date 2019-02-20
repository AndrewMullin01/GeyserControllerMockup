/*
 * adc.c
 *
 *  Created on: 12 Mar 2018
 *      Author: 19837739
 */

#include "adc.h"
#include "SevSeg.h"
#include "uart.h"
#include <math.h>

uint16_t adc1_Val[4];

uint16_t CH1_midVal = 0;
uint16_t CH1_oldVal = 0;

uint16_t CH2_midVal = 0;
uint16_t CH2_oldVal = 0;

int fastCurrentRMS = 0;
int fastVoltageRMS = 0;
int currentRMS = 0;
int voltageRMS = 0;

uint8_t sumCount = 0;
float rmsCurrentSum = 0;
float rmsVoltageSum = 0;
float sampleCurrent = 0;
float sampleVoltage = 0;

uint16_t ambientTemp = 0;
uint16_t heaterTemp = 0;


void adcInit()
{

}

void adcLoop()
{
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc1_Val, 4);
	//fastFindMax(adc1_Val);
	findRMS(adc1_Val);
	findTemperature(adc1_Val);


}

void fastFindMax(uint16_t* newVal)
{

	if(CH1_midVal > newVal[0] && CH1_midVal > CH1_oldVal)
	{
		fastCurrentRMS = map((float)CH1_midVal, adc_curr_min, adc_curr_max, -13000, 13000)*0.70711;
	}
	CH1_oldVal = CH1_midVal;
	CH1_midVal = newVal[0];

	if(CH2_midVal > newVal[1] && CH2_midVal > CH2_oldVal)
	{
		fastVoltageRMS = map((float)CH2_midVal, adc_volt_min, adc_volt_max, -220000, 220000)*0.70711;
	}
	CH2_oldVal = CH2_midVal;
	CH2_midVal = newVal[1];

}

void findRMS(uint16_t* newVal)
{
	sampleCurrent = map((float)newVal[0], adc_curr_min, adc_curr_max, -13000, 13000); // normalise

	rmsCurrentSum = rmsCurrentSum + (sampleCurrent*sampleCurrent);

	sampleVoltage = map((float)newVal[1], adc_volt_min, adc_volt_max, -220000, 220000); // normalise

	rmsVoltageSum = rmsVoltageSum + (sampleVoltage*sampleVoltage);

	if(++sumCount == N)
	{
		voltageRMS = sqrt(rmsVoltageSum/N);
		currentRMS = sqrt(rmsCurrentSum/N);

		rmsVoltageSum = 0;
		rmsCurrentSum = 0;
		sumCount = 0;
	}

}

void findTemperature(uint16_t* newVal)
{


	heaterTemp = ((((float)newVal[2]*3.3/4096) - 0.5)/0.01) -2;
	ambientTemp = (((float)newVal[3]*3.3/4096) - 0.5)/0.01;



}

// taken from arduino source code
float map(float x, float in_min, float in_max, float out_min, float out_max)
{
	return (x - in_min)*(out_max - out_min)/(in_max - in_min) + out_min;
}
