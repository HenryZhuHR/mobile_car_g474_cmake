#ifndef __SENSOR_BASIC_H__
#define __SENSOR_BASIC_H__

#include "stm32g4xx_hal.h"

#define OFFSET_AV_NUM 50
#define GRAVITY_ACC_PN16G 2048
#define RANGE_PN2000_TO_RAD 0.00908f
#define RANGE_PN16G_TO_CMSS 0.4790f

enum
{
	ROL,
	PIT,
	TAW,
	VEC_RPY,
};

enum
{
	X,
	Y,
	Z,
	VEC_XYZ,
};

enum
{
	A_X = 0,
	A_Y,
	A_Z,
	G_X,
	G_Y,
	G_Z,
	TEM,
	MPU_ITEMS,
};

typedef struct
{
	int x;
	int y;
} _vector2_st;

typedef struct
{
	int x;
	int y;
	int z;
} _vector3_st;

enum ROBOT_STATUS
{
	MODE_REMOTE_CTRL = 0, // 遥控控制
	MODE_UART_CTRL,		  // 串口控制
};

typedef struct
{
	uint8_t sensor_imu_ok;
	uint8_t mems_temperature_ok;

	uint8_t motionless;

	uint32_t remote_ctrl_timestamp;
	uint8_t robot_sta; // 机器人状态
	uint8_t low_power; // 电量

} _flag_st;
extern _flag_st flag;

typedef struct
{
	uint8_t surface_CALIBRATE;
	float surface_vec[VEC_XYZ];
	float surface_unitvec[VEC_XYZ];

} _sensor_rotate_st;
extern _sensor_rotate_st sensor_rot;

typedef struct
{
	// 校准参数
	uint8_t acc_CALIBRATE;
	uint8_t gyr_CALIBRATE;
	uint8_t acc_z_auto_CALIBRATE;

	// 原始数据
	int16_t Acc_Original[VEC_XYZ];
	int16_t Gyro_Original[VEC_XYZ];

	int16_t encoder_incre[4]; // 4个编码器的值

	int16_t Acc[VEC_XYZ];
	int32_t Acc_cmss[VEC_XYZ]; // 单位：cm/s2
	float Gyro[VEC_XYZ];
	float Gyro_deg[VEC_XYZ]; // 单位：角度
	float Gyro_rad[VEC_XYZ]; // 单位：弧度
	float gyro_rps[3];
	float accel_mpss[3];

	int16_t Tempreature;
	float Tempreature_C; // 单位：℃

} _sensor_st; //__attribute__((packed))

extern _sensor_st sensor;

//==函数声明

// public
void Sensor_Data_Prepare(uint8_t dT_ms);
void Sensor_Get(void);

#endif
