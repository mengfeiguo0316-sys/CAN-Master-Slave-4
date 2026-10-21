#include "uart_report.h"
#include "usart.h"      /* huart1 */
#include "loop.h"       /* g_motor_mode */
#include "PID.h"        /* PID[]   */
#include "Getspeed.h"   /* Circlemax */
#include <stdio.h>      /* snprintf */
#include <string.h>     /* strlen   */
#include <math.h>       /* lroundf  */

/* 注意：工程链接使用 --specs=nano.specs，nano-printf 不支持 %f
 * 所以浮点都用"整数部分 + 两位小数"的方式输出 */

#define UART_TX_BUF_SIZE   128

/* 发送缓冲区必须是 static：中断方式发送期间，这块内存必须一直有效 */
static char s_tx_buf[UART_TX_BUF_SIZE];

/**
 * @brief  串口上报初始化：打开 USART1 的 NVIC 中断
 */
void UART_Report_Init(void)
{
  /* 优先级 5：低于 CAN(0)，高于 SysTick(15)，ISR 很短不会影响控制周期 */
  HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(USART1_IRQn);
}

/**
 * @brief  非阻塞发送：上次还没发完就直接丢弃本次
 */
static void UART_SendAsync(const char *s, uint16_t len)
{
  if (len == 0) return;

  /* gState 不是 READY 说明上一条还在发送中（或串口未初始化），本次跳过 */
  if (huart1.gState != HAL_UART_STATE_READY) return;

  HAL_UART_Transmit_IT(&huart1, (uint8_t *)s, len);
}

/**
 * @brief  开机自检信息（非阻塞）
 */
void UART_Report_Boot(void)
{
  const char *boot = "\r\n[BOOT] CAN_Send ready, UART1 115200 8N1 (IT mode)\r\n";
  UART_SendAsync(boot, (uint16_t)strlen(boot));
}

/**
 * @brief  取小数部分（两位），配合 %d.%02d 输出浮点
 */
static int frac2(float v)
{
  float i = (float)(int)v;
  return (int)lroundf((v - i) * 100.0f);
}

/**
 * @brief  上报电机状态（非阻塞，函数立即返回）
 * @note   输出示例：
 *           mode=1, speed=200 RPM, pos=8123 pulse (3535 deg), pwm=150, Kp=1.00 Ki=0.02 Kd=0.00
 */
void UART_Report_MotorState(void)
{
  int len;

  /* 转速：四舍五入取整 */
  float v = PID[0].speed_now;
  int   speed_rpm = (int)(v >= 0.0f ? (v + 0.5f) : (v - 0.5f));

  int   pos_pulse = (int)PID[0].pos_now;                          /* 编码器脉冲 */
  int   pos_deg   = (int)(PID[0].pos_now * 360.0f / Circlemax);   /* 换算成角度 */
  int   pwm_duty  = (int)PID[0].output;                           /* 0~999 */

  len = snprintf(s_tx_buf, sizeof(s_tx_buf),
                 "mode=%d, speed=%d RPM, pos=%d pulse (%d deg), pwm=%d, "
                 "Kp=%d.%02d Ki=%d.%02d Kd=%d.%02d\r\n",
                 (int)g_motor_mode, speed_rpm, pos_pulse, pos_deg, pwm_duty,
                 (int)PID[0].Kp, frac2(PID[0].Kp),
                 (int)PID[0].Ki, frac2(PID[0].Ki),
                 (int)PID[0].Kd, frac2(PID[0].Kd));

  if (len > 0)
  {
    /* snprintf 返回的是"理论长度"，这里做一次截断保护 */
    if (len >= (int)sizeof(s_tx_buf)) len = (int)sizeof(s_tx_buf) - 1;
    UART_SendAsync(s_tx_buf, (uint16_t)len);
  }
}

/**
 * @brief  USART1 中断服务函数
 * @note   放在本文件而不是 stm32f1xx_it.c，避免被 CubeMX 覆盖。
 *         如果以后在 CubeMX 里给 USART1 勾选了 NVIC 中断，
 *         CubeMX 会在 stm32f1xx_it.c 里再生成一份同名函数 —— 那时请删掉这里。
 */
void USART1_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart1);
}
