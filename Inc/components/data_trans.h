
#ifndef __DATA_TRANS_H__
#define __DATA_TRANS_H__
#include "stm32g4xx_hal.h"

extern uint8_t data_receive[100];
extern uint8_t data_one_byte[1];
void GetOneByte(uint8_t data);
void DataDecoder(uint8_t *data, uint8_t len);

extern UART_HandleTypeDef hlpuart1;
extern UART_HandleTypeDef huart1;
void DataTrans(void);
void DataTrans_IMU_Raw(void);
void DataTrans_UserData(void);
void DataTrans_Odom(void);
void DataTrans_Task(uint32_t dT_ms);


#endif
