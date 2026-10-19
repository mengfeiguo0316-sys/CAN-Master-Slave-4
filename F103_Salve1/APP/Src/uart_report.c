#include "uart_report.h"
#include "usart.h"      /* huart1 */
#include "loop.h"       /* g_motor_mode */
#include "PID.h"        /* PID[]   */
#include "Getspeed.h"   /* Circlemax */
#include <stdio.h>      /* snprintf */

/* 注意：工程链接使用 --specs=nano.specs，nano-printf 不支持 %f
 * 所以这里全部用整型输出（转速取整、位置同时给脉冲和角度） */

void UART_Report_MotorState(void)
{
  char buf[96];
  int  len;

  /* 转速：四舍五入取整 */
  float v = PID[0].speed_now;
  int   speed_rpm = (int)(v >= 0.0f ? (v + 0.5f) : (v - 0.5f));

  int   pos_pulse = (int)PID[0].pos_now;                          /* 编码器脉冲 */
  int   pos_deg   = (int)(PID[0].pos_now * 360.0f / Circlemax);   /* 换算成角度 */
  int   pwm_duty  = (int)PID[0].output;                           /* 0~999 */

  len = snprintf(buf, sizeof(buf),
                 "mode=%d, speed=%d RPM, pos=%d pulse (%d deg), pwm=%d\r\n",
                 (int)g_motor_mode, speed_rpm, pos_pulse, pos_deg, pwm_duty);

  if (len > 0)
  {
    /* 阻塞发送，超时 100ms；115200 波特率下一行约 5ms */
    HAL_UART_Transmit(&huart1, (uint8_t *)buf, (uint16_t)len, 100);
  }
}
