/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"
/* Exported types ------------------------------------------------------------*/

typedef struct
{
	void (*task_func)(uint32_t dT_us);
	int interval_ticks;
	uint64_t last_run;
} sched_task_t;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

// static
uint32_t GetSysRunTimeUs(void);
// user
uint8_t Main_Task(void);
void INT_1ms_Task(void);
#endif
