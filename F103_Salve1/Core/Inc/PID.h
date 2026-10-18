#pragma once
#include "main.h"

typedef struct
{
	//基本参数
	float Kp;
	float Ki;
	float Kd;
	
	float pos_kp;
	float pos_now;
	
	//误差值
	float ei;
	float eilast;
	float eisum;
	float eidif;
	
	
	
	//积分限幅,输出限幅
	float Points_max;
	float Points_min;
	float out_max;
	float out_min;
	float output;
	
	//速度值
	float speed_now;
	float speed_goal;//中间量
	float speed_real;//真实目标速度
	float speed_step;//步长
	
	
}PIDdata;

extern PIDdata PID[3];//0~2对应速度，位置，电流环
void PID_init(PIDdata* pid,float kp,float ki,float kd);
void PID_Speed(PIDdata* pid,float step,float real);	
void PID_calculate(PIDdata* pid);
void PID_Clear(PIDdata* pid);

