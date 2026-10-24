#pragma once
#include "main.h"
#include "PID.h"
#include "tim.h"

#define Circlemax 827.2f

typedef struct num{
	uint16_t NowNum;
	uint16_t BefNum;
	int16_t ChangeNum;
	
	uint16_t Nowpos;
	uint16_t Befpos;
	int16_t Changepos;
	
}Mc;


void Mcinit(void);
void Getspeed(PIDdata* pid);
void Getpos(PIDdata* pid);
