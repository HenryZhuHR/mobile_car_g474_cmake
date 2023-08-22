#ifndef __BATTERY_H__
#define __BATTERY_H__

#include <stdint.h>
#include "stm32g4xx_hal.h"


extern ADC_HandleTypeDef hadc2;

void Battery_Init();
void Battery_Task(uint32_t dT_ms);

#endif