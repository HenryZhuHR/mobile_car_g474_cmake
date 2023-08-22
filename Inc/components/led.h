#ifndef __LED_H
#define __LED_H

#include "stm32g4xx_hal.h"

void Drv_LED_Init(void);
void LED_1ms_DRV(void );
void LED_Task(uint8_t dT_ms);
void LED_Task2(int8_t dT_ms);

#endif
