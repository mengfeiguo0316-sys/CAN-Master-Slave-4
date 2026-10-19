#include "PID.h"

PIDdata PID[3];

/**
 * @brief  PID数据初始化
 * 
 * @param  PID选择
 * @param  初始化kp值
 * @param  初始化ki值
 * @param  初始化kd值
 *
 * @retval 无
 * 
 * @note   
 */
void PID_init(PIDdata* pid,float kp,float ki,float kd)
{
	pid->Kp = kp;
	pid->Ki = ki;
	pid->Kd = kd;
	pid->eilast = 0;
	
	//积分限幅初始化
	pid->Points_max = 5000;
	pid->Points_min = -5000;
	//输出限幅初始化
	pid->out_max = 999;
	pid->out_min = -999;
	
}

/**
 * @brief  速度设置及计算
 *
 * @param  PID选择
 * @param  速度改变步长
 * @param  真实目标速度
 * 
 * @retval 无
 * 
 * @note   
 */
void PID_Speed(PIDdata* pid,float step,float real)
{
	pid->speed_step = step;
	pid->speed_real = real;
	
	if(pid->speed_goal < pid->speed_real)
	{
		pid->speed_goal += pid->speed_step; // 加上步长
		if(pid->speed_goal > pid->speed_real)
		{
			pid->speed_goal = pid->speed_real;
		}
	}
	else if(pid->speed_goal > pid->speed_real)
	{
		pid->speed_goal -= pid->speed_step;//减去步长
		if(pid->speed_goal < pid->speed_real)
		{
			pid->speed_goal = pid->speed_real;
		}
		
	}
	
}

//PID的相关计算
void PID_calculate(PIDdata* pid)
{
	//误差计算
	pid->ei = pid->speed_goal - pid->speed_now;
	//防止误差值不断累加
	if(pid->ei > 1000)
	{
		pid->ei = 1000;
	}
	else if(pid->ei < -1000)
	{
		pid->ei = -1000;
	}
	//误差累计计算
	
	pid->eisum += pid->ei;
	
	if(pid->eisum < pid->Points_min )
	{
		pid->eisum = pid->Points_min;
	}
	else if(pid->eisum > pid->Points_max)
	{
		pid->eisum = pid->Points_max;
	}
	
	//误差差值计算
	pid->eidif = pid->ei - pid->eilast;
	pid->eilast = pid->ei;
	
	//PID计算
	pid->output = (pid->Kp * pid->ei) + (pid->Ki * pid->eisum) + (pid->Kd * pid->eidif);
	if(pid->output < pid->out_min)
	{
		pid->output = pid->out_min;
	}
	else if(pid->output > pid->out_max)
	{
		pid->output = pid->out_max;
	}
	
}


//清空 PID 积分与历史误差（急停或切换模式时调用）
void PID_Clear(PIDdata* pid)
{
    pid->ei         = 0.0f;
    pid->eilast     = 0.0f;
    pid->eisum      = 0.0f; // 核心：清除积分累加
    pid->eidif      = 0.0f;
    pid->speed_goal = 0.0f; // 清除速度斜坡中间量
    pid->output     = 0.0f;
}