#include "Getspeed.h"

Mc data;

void Mcinit(void)
{
	data.NowNum = 0;
	data.BefNum = 0;
	data.ChangeNum = 0;
	data.Nowpos = 0;
	data.Befpos = 0;  
	data.Changepos = 0;
	
}

void Getspeed(PIDdata* pid)
{
	
		data.NowNum = __HAL_TIM_GET_COUNTER(&htim1);
		data.ChangeNum = (int16_t)(data.NowNum - data.BefNum);
		data.BefNum = data.NowNum;
		
		// 计算本次的瞬时速度
		float current_speed = data.ChangeNum * 6000.0f / Circlemax;
    
		// 一阶低通滤波 (0.3是信任新数据的权重，0.7是保留旧数据的权重，可微调)
		pid->speed_now = (0.7f * pid->speed_now) + (0.3f * current_speed);
		
}

void Getpos(PIDdata* pid)
{
	
	data.Nowpos = __HAL_TIM_GET_COUNTER(&htim1);
    
    data.Changepos = (int16_t)(data.Nowpos - data.Befpos);
    data.Befpos = data.Nowpos;
    
    pid->pos_now += data.Changepos; 
	
}

