/**
 * @file dt.c
 * @brief 数据传输程序
 * @author Linfu Wei (ghowoght@qq.com)
 * @version 1.0
 * @date 2020-12-27
 *
 * @copyright Copyright (c) 2020  WHU-EIS
 *
 */
#include "stdio.h"
#include "components/data_trans.h"
#include "components/imu.h"
#include "components/sensor_basic.h"
#include "components/motor.h"
#include "components/user_ctrl.h"

/**
 ************************
 ******* 发送数据 ********
 ************************
 */

#define USE_USB

/**
 * @brief 数据发送函数
 * @param  data_to_send     待发送的字节数组
 * @param  cnt              待发送数据长度
 */
void SendData(uint8_t *data_to_send, uint8_t cnt)
{
#ifdef USE_USB
	/* 关闭串口接收中断 */
	HAL_NVIC_DisableIRQ(LPUART1_IRQn);
	/* 发送数据 */
	HAL_UART_Transmit(&hlpuart1, data_to_send, cnt, 0xFFFF);
	/* 开启串口接收中断 */
	HAL_NVIC_EnableIRQ(LPUART1_IRQn);
#else
	/* 关闭串口接收中断 */
	HAL_NVIC_DisableIRQ(USART1_IRQn);
	/* 发送数据 */
	HAL_UART_Transmit(&huart1, data_to_send, cnt, 0xFFFF);
	/* 开启串口接收中断 */
	HAL_NVIC_EnableIRQ(USART1_IRQn);
#endif
}

// 联合体，用于float型和字节数组的互换
typedef union
{
	float data;
	uint8_t data_array[4];
} data_u_float;

/**
 * @brief 数据发送任务
 * @param  dT_ms            执行周期
 */
void DataTrans_Task(uint32_t dT_ms)
{
	static uint32_t cnt = 0;
	const uint32_t sent_imu_cnt = 1;
	const uint32_t sent_odom_cnt = 20;
	const uint32_t sent_userdata_cnt = 40;
	const uint32_t sent_wheel_cnt = 30;

	cnt += dT_ms;

	// if((cnt % sent_odom_cnt) == sent_odom_cnt - 1)
	// {
	// 	DataTrans_Odom();
	// }
	// else if((cnt % sent_userdata_cnt) == sent_userdata_cnt - 1)
	// {
	// 	DataTrans_UserData();
	// }
	if ((cnt % sent_imu_cnt) == sent_imu_cnt - 1)
	{
		DataTrans_IMU_Raw();
	}

	if (cnt > 1200)
		cnt = 0;
}

/**
 * @brief 发送IMU原始数据, 包含里程计数据
 */
void DataTrans_IMU_Raw(void)
{
	uint8_t _cnt = 0;
	data_u_float _temp;
	uint8_t data_to_send[100] = {0};

	data_to_send[_cnt++] = 0xAA;
	data_to_send[_cnt++] = 0x55;

	uint8_t _start = _cnt;

	// IMU数据序号
	for (int i = 0; i < 4; i++)
	{
		data_to_send[_cnt++] = *((uint8_t *)(&imu_data_cnt) + i);
	}

	// IMU数据打包
	for (int i = 0; i < 6; i++)
	{
		data_to_send[_cnt++] = *((uint8_t *)(sensor.Gyro_Original) + i);
	}
	for (int i = 0; i < 6; i++)
	{
		data_to_send[_cnt++] = *((uint8_t *)(sensor.Acc_Original) + i);
	}

	// 里程计数据打包
	for (int i = 0; i < 8; i++)
	{
		data_to_send[_cnt++] = *((uint8_t *)(sensor.encoder_incre) + i);
	}

	uint8_t checkout = 0;
	for (int i = _start; i < _cnt; i++)
	{
		checkout += data_to_send[i];
	}
	data_to_send[_cnt++] = checkout;
	// 串口发送
	SendData(data_to_send, _cnt);
	
}
/**
 * @brief 发送用户自定义数据
 */
void DataTrans_UserData(void)
{
	uint8_t _cnt = 0;
	data_u_float _temp;				 // 声明一个联合体实例，使用它将待发送数据转换为字节数组
	uint8_t data_to_send[100] = {0}; // 待发送的字节数组

	data_to_send[_cnt++] = 0xAA;
	data_to_send[_cnt++] = 0x55;
	data_to_send[_cnt++] = 0x04; // 类型
	data_to_send[_cnt++] = 16;	 // 长度

	uint8_t _start = _cnt;

	float datas[] = {kinematics.fb_wheel_rpm.motor_1,
					 kinematics.fb_wheel_rpm.motor_2,
					 kinematics.fb_wheel_rpm.motor_3,
					 kinematics.fb_wheel_rpm.motor_4};

	for (int i = 0; i < sizeof(datas) / sizeof(float); i++)
	{
		// 将要发送的数据赋值给联合体的float成员
		// 相应的就能更改字节数组成员的值
		_temp.data = datas[i];
		data_to_send[_cnt++] = _temp.data_array[0];
		data_to_send[_cnt++] = _temp.data_array[1];
		data_to_send[_cnt++] = _temp.data_array[2];
		data_to_send[_cnt++] = _temp.data_array[3]; // 最高位
	}

	uint8_t checkout = 0;
	for (int i = _start; i < _cnt; i++)
	{
		checkout += data_to_send[i];
	}
	data_to_send[_cnt++] = checkout;
	// 串口发送
	// SendData(data_to_send, _cnt);
}

/**
 * @brief 发送里程计数据
 */
void DataTrans_Odom(void)
{
	uint8_t _cnt = 0;
	data_u_float _temp;				 // 声明一个联合体实例，使用它将待发送数据转换为字节数组
	uint8_t data_to_send[100] = {0}; // 待发送的字节数组

	data_to_send[_cnt++] = 0xAA;
	data_to_send[_cnt++] = 0x55;

	uint8_t _start = _cnt;

	float datas[] = {kinematics.odom.vel.linear_x,
					 kinematics.odom.vel.linear_y,
					 kinematics.odom.vel.angular_z,
					 kinematics.odom.pose.theta};

	for (int i = 0; i < sizeof(datas) / sizeof(float); i++)
	{
		// 将要发送的数据赋值给联合体的float成员
		// 相应的就能更改字节数组成员的值
		_temp.data = datas[i];
		data_to_send[_cnt++] = _temp.data_array[0];
		data_to_send[_cnt++] = _temp.data_array[1];
		data_to_send[_cnt++] = _temp.data_array[2];
		data_to_send[_cnt++] = _temp.data_array[3]; // 最高位
	}

	uint8_t checkout = 0;
	for (int i = _start; i < _cnt; i++)
	{
		checkout += data_to_send[i];
	}
	data_to_send[_cnt++] = checkout;
	// 串口发送
	// SendData(data_to_send, _cnt);
}

/**
 ************************
 ******* 接收数据 ********
 ************************
 */

uint8_t data_receive[100];
uint8_t data_one_byte[1];
/**
 * @brief 串口回调函数
 * @param  huart            串口句柄
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		GetOneByte(data_one_byte[0]);
		// 在完成一次接收后，串口中断会被关闭，需要再次打开
		HAL_UART_Receive_IT(&huart1, data_one_byte, 1);
	}

	if (huart->Instance == LPUART1)
	{
		GetOneByte(data_one_byte[0]);
		// 在完成一次接收后，串口中断会被关闭，需要再次打开
		HAL_UART_Receive_IT(&hlpuart1, data_one_byte, 1);
	}
}

/**
 * @brief 从串口读取单个字节
 * @param  data             读取的字节数据
 */
void GetOneByte(uint8_t data)
{
	static uint8_t state = 0;	 // 接收状态
	static uint8_t data_cnt = 0; // 接收到的数据长度

	if (state == 0 && data == 0xAA)
	{
		data_cnt = 0;
		state = 1;
		data_receive[data_cnt++] = data;
	}
	else if (state == 1 && data == 0x55)
	{
		data_receive[data_cnt++] = data;
		state++;
	}
	else if (state == 2)
	{
		data_receive[data_cnt++] = data;

		if (data == 0xFF && data_cnt >= 18)
		{
			SendData(data_receive, data_cnt);
			/**
			 * 运动控制串口数据帧设计
			 * | 数据位  | 数据说明     |
			 * | :----- | ------------ |
			 * | 0      | 帧头1 `0xAA` |
			 * | 1      | 帧头2 `0x55` |
			 * | 2      | 数据类型标记   |
			 * | 3      | 保留         |
			 * | 4      | x 运动速度	    |
			 * | 5      | y 运动速度    |
			 * | 6      | z 运动速度    |
			 * | 7      | z 运动速度	|
			 * | 8      | z 运动速度	|
			 * | 9      | z 运动速度	|
			 * | 10     | 保留			|
			 * | 11     | 保留			|
			 * | 12     | 保留			|
			 * | 13     | 保留			|
			 * | 14     | 保留			|
			 * | 15     | 保留			|
			 * | 16     | 校验和		|
			 * | 17     | 帧尾 `0xFF` 	|
			 *
			 *
			 * - x,y 速度  int8 类型 -128～127 刚好就是 1.3
			 * - z   速度 float 类型 不然精度不够
			 * - 不可以发送太长，否则接收不到
			 */

			// 校验
			uint8_t checkout = 0;
			for (int i = 2; i < data_cnt - 2; i++)
			{
				checkout += data_receive[i];
			}
			if (checkout == data_receive[data_cnt - 2])
			{
				// 校验通过，进行解码
				
				DataDecoder(data_receive, 20);
				HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
			}

			state = 0;
			data_cnt = 0;
		}
	}
	else
	{
		state = 0;
		data_cnt = 0;
	}
}

// 单片机复位指令
uint8_t reset_checkout[12] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC};
/**
 * @brief 数据解码
 * @param  data 待解码数组
 */
void DataDecoder(uint8_t *data, uint8_t len)
{
	uint8_t _cnt = 0;

	if (flag.robot_sta == MODE_UART_CTRL)
	{
		int8_t _temp = 0;
		_temp = data[4];
		kinematics.exp_vel.linear_x = _temp / 100.0f;
		_temp = data[5];
		kinematics.exp_vel.linear_y = _temp / 100.0f;

		data_u_float _temp_float;
		_cnt = 0;
		_temp_float.data_array[_cnt++] = data[6];
		_temp_float.data_array[_cnt++] = data[7];
		_temp_float.data_array[_cnt++] = data[8];
		_temp_float.data_array[_cnt++] = data[9];
		kinematics.exp_vel.angular_z = _temp_float.data;
	}

	// 复位指令校验
	for (int i = 0; i < 12; i++)
	{
		if (data[i] != reset_checkout[i])
			return;
	}
	HAL_NVIC_SystemReset();
}
