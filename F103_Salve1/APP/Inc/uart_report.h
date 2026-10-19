#ifndef __UART_REPORT_H
#define __UART_REPORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/**
 * @brief  通过串口(USART1, 115200 8N1)上报一次电机状态
 * @note   输出格式（一行，\r\n 结尾）：
 *           mode=1, speed=50 RPM, pos=1234 pulse (537 deg), pwm=350
 *           mode : 当前控制模式 0=停机 1=速度环 2=单位置环 3=双环
 *           speed: 当前实际转速（RPM，已滤波）
 *           pos  : 当前累计位置（编码器脉冲）
 *           deg  : 换算后的角度（度）
 *           pwm  : 当前 PWM 占空比（0~999）
 */
void UART_Report_MotorState(void);

#ifdef __cplusplus
}
#endif

#endif /* __UART_REPORT_H */
