#include "loop.h"


#define PWM_SLEW_STEP   50.0f

/**
 * @brief  初始化 PWM 与电机驱动
 * @note   启动 TIM2 通道2 的 PWM 输出，初始占空比设为 0（防止上电瞬间电机转动），
 *         并拉高 STBY 引脚使能 TB6612 驱动芯片
 */
void PWM_init(void)
{
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);// 启动 TIM2 通道 2 的 PWM
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);// 占空比设为0，确保上电安全
	HAL_GPIO_WritePin(STBY_GPIO_Port,STBY_Pin,1);// 使能 TB6612
}

/**
 * @brief  根据 PID 输出值设置电机转向与 PWM 占空比
 * @param  pid: PID 数据结构体指针（使用其 output 成员）
 * @note   TB6612 控制逻辑：
 *           AIN1=1, AIN2=0 → 正转
 *           AIN1=0, AIN2=1 → 反转
 *           AIN1=1, AIN2=1 → 刹车（短路制动）
 */
void PWM_set(PIDdata* pid)
{
	
	if (pid->output > 0)
    {
        // 正转 (AIN1=1, AIN2=0)
        HAL_GPIO_WritePin(AIN1_GPIO_Port, AIN1_Pin,1);
        HAL_GPIO_WritePin(AIN2_GPIO_Port, AIN2_Pin, 0);
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, pid->output);
    }
    else if (pid->output < 0)
    {
        // 反转 (AIN1=0, AIN2=1)
        HAL_GPIO_WritePin(AIN1_GPIO_Port, AIN1_Pin, 0);
        HAL_GPIO_WritePin(AIN2_GPIO_Port, AIN2_Pin, 1);
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, -pid->output); // 取绝对值（占空比不能为负）
    }
	else // 当 output 为 0 时，设置为刹车
    {
        HAL_GPIO_WritePin(AIN1_GPIO_Port, AIN1_Pin, 1);
        HAL_GPIO_WritePin(AIN2_GPIO_Port, AIN2_Pin, 1); // 1、1 导通，进入 TB6612 刹车模式
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, pid->output);
    }
	
}

/**
 * @brief  设置位置环的 KP 参数
 * @param  pid: PID 数据结构体指针
 * @param  kp:  位置环比例系数
 */
void pos_kp(PIDdata* pid,float kp)
{
	pid->pos_kp = kp;
}

/**
 * @brief  位置环计算（角度 → PWM 映射）
 * @param  pid:      PID 数据结构体指针
 * @param  pos_goal: 目标角度（单位：度）
 * @retval 位置环输出的速度给定值
 */
float POS (PIDdata* pid,float pos_goal)
{	
	pos_goal = pos_goal * Circlemax / 360;// 将角度换算为 PWM 映射值
	
	float pid_eipos = pos_goal - pid->pos_now ;// 位置环误差
	
	float pid_pos = pid->pos_kp * pid_eipos;// 位置环 P 输出
	
	return pid_pos;
}

/**
 * @brief  速度-位置串级控制（位置环作外环，速度环作内环）
 * 
 * @param  moed:   电机控制模式
 * @param  pos:    目标位置（角度）
 * @param  speed:  目标速度
 *
 * @retval 无
 * 
 * @note   位置环开启时：位置环的输出作为速度环的给定，并限幅在 ±300；
 *         位置环关闭时：直接使用传入的 speed 作为速度环给定
 */

MotorCtrlMode_e g_motor_mode = MOTOR_MODE_STOP;//设置默认状态为停机模式
float g_target_pos   = 0.0f;
float g_target_speed = 0.0f;

void Speed_Pos_loop(MotorCtrlMode_e mode,float pos,float speed)
{		
		// 1. 无论处于什么模式，都实时更新当前速度和位置，确保 CAN (0x201) 随时能上报最新状态
    	Getspeed(&PID[0]);
    	Getpos(&PID[0]);
	
		//2.根据不同模式计算PID输出
		switch	(mode)
		{
			//停机(急停)模式(0x00)
			case MOTOR_MODE_STOP:
				PID_Clear(&PID[0]);    // 清空积分项和输出，防止下次启动暴冲
        		break;

			//单速度环模式(0x01)
			case MOTOR_MODE_SPEED:
				// 速度目标限幅
				if (speed >  500.0f) speed =  500.0f;
				if (speed < -500.0f) speed = -500.0f;
				PID_Speed(&PID[0],5.0f,speed);
				PID_calculate(&PID[0]);
				break;

			//单位置环模式(0x02)
			case MOTOR_MODE_POSITION:
			{
				// 直接将位置环P的输出作为 PWM 占空比，不经过速度环 PID_calculate
           		float pwm_out = POS(&PID[0], pos);
				// PWM 占空比限幅
            	if (pwm_out > 999.0f)  pwm_out = 999.0f;
            	if (pwm_out < -999.0f) pwm_out = -999.0f;

				// 平滑过渡：限制每次更新的 PWM 变化量，防止电机突然加速或减速
				float out = PID[0].output; 
				if (pwm_out > out + PWM_SLEW_STEP) out += PWM_SLEW_STEP;   // 加速上升
				else if (pwm_out < out - PWM_SLEW_STEP) out -= PWM_SLEW_STEP;   // 减速下降
				else  out  = pwm_out;     // 已接近，直接跟上

				PID[0].output = out;
            	break;
			}
			//位置-速度双环模式(0x03)
			case MOTOR_MODE_POS_SPEED:
				// 外环：位置环输出作为速度环的目标值
				speed = POS(&PID[0], pos);
				// 速度目标限幅
				if (speed >  500.0f) speed =  500.0f;
				if (speed < -500.0f) speed = -500.0f;
				// 内环：速度环计算
				PID_Speed(&PID[0],5.0f,speed);
            	PID_calculate(&PID[0]);
            	break;

			default:
            	PID[0].output = 0.0f;
            	break;		
		}

		// 3. 将计算出的 output 作用到电机驱动引脚
    	PWM_set(&PID[0]);
}