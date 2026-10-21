#pragma once
#include "main.h"
#include "PID.h"
#include "tim.h"
#include "Getspeed.h"
#include "can_protocol.h"

void pos_kp(PIDdata* pid,float kp);
float POS (PIDdata* pid,float pos_goal);
void PWM_init(void);
void PWM_set(PIDdata* pid);
void Speed_Pos_loop(MotorCtrlMode_e mode,float pos,float speed);

extern MotorCtrlMode_e g_motor_mode;
extern float g_target_pos;
extern float g_target_speed;
