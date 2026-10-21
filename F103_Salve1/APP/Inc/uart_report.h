#ifndef __UART_REPORT_H
#define __UART_REPORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/**
 * @brief  串口上报初始化
 * @note   打开 USART1 的 NVIC 中断，HAL_UART_Transmit_IT() 才能正常工作。
 *         必须在 MX_USART1_UART_Init() 之后调用一次。
 */
void UART_Report_Init(void);

/**
 * @brief  开机自检：上电后发一行固定信息（非阻塞）
 */
void UART_Report_Boot(void);

/**
 * @brief  通过串口(USART1, 115200 8N1)上报一次电机状态（非阻塞）
 * @note   输出格式（一行，\r\n 结尾）：
 *           mode=1, speed=50 RPM, pos=1234 pulse (537 deg), pwm=350
 *           mode : 当前控制模式 0=停机 1=速度环 2=单位置环 3=双环
 *           speed: 当前实际转速（RPM，已滤波）
 *           pos  : 当前累计位置（编码器脉冲）
 *           deg  : 换算后的角度（度）
 *           pwm  : 当前 PWM 占空比（0~999）
 *
 *         采用中断方式发送，函数立即返回，不阻塞主循环。
 *         若上一次还没发完，则本次直接丢弃（5 秒一次，丢一次无影响）。
 */
void UART_Report_MotorState(void);

#ifdef __cplusplus
}
#endif

#endif /* __UART_REPORT_H */
