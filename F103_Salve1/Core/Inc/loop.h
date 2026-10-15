#pragma once
#include "main.h"
#include "PID.h"
#include "tim.h"
#include "Getspeed.h"

void pos_kp(PIDdata* pid,float kp);
float POS (PIDdata* pid,float pos_goal);
void PWM_init(void);
void PWM_set(PIDdata* pid);
void Speed_Pos_loop(uint8_t pos_ok,float pos,float speed);

extern volatile uint8_t ok;
