/*
 * adc.h
 *
 *  Created on: 12 Mar 2018
 *      Author: 19837739
 */

#ifndef ADC_H_
#define ADC_H_

#include "main.h"
#include "stm32f3xx_hal.h"

#define N 20 // number of samples taken

//[0, 3724]
#define adc_volt_max 3724//3616
#define adc_volt_min 0//60
#define adc_curr_max 3724//3565//3728
#define adc_curr_min 0//5
#define ADC11_OFFSET 10

extern int it_count;
extern ADC_HandleTypeDef hadc1;

void adcInit();

void adcLoop();

void fastFindMax(uint16_t* newVal);

void findRMS(uint16_t* newVal);

void calcVoltageRMS(uint16_t peak);

void calcCurrentRMS(uint16_t peak);

void findTemperature(uint16_t* newVal);

float map(float x, float in_min, float in_max, float out_min, float out_max);

#endif /* ADC_H_ */
