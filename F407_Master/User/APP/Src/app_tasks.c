#include "app_tasks.h"
#include "bsp_key.h"
#include "bsp_ds18b20.h"
#include "bsp_led.h"
#include "can.h"
#include "can_protocol.h"
#include "usart.h"
#include "Delay_us.h"
#include <stdio.h>
#include <string.h>

CanMsg_MotorCmd_t motor_cmd={0};

/*软实时任务：允许几毫秒甚至十几毫秒的抖动，人眼和机械按键完全察觉不出。*/

void Tasks_Init(void)
{
  Delay_Init();
  BSP_LED_Init();
  BSP_DS18B20_Init();
}

void key1_task(void)
{
  if (BSP_KEY_Scan(KEY_ID_0) == KEY_SHORT)
    {
        motor_cmd.ctrl_mode = MOTOR_MODE_SPEED;
        motor_cmd.target_speed = 50;
        motor_cmd.cmd_seq++;
        CAN_Send_cmd(&motor_cmd);
    }
}

void key2_task(void)
{
  if (BSP_KEY_Scan(KEY_ID_1) == KEY_SHORT)
    {
      float tem = BSP_DS18B20_ReadTemp();                           
      int16_t temp_send = (int16_t)(tem >= 0.0f ? (tem * 10.0f + 0.5f) : (tem * 10.0f - 0.5f));

      char buf[32];
      int len = snprintf(buf, sizeof(buf), "TEMP=%d\r\n", temp_send);
      HAL_UART_Transmit(&huart1, (uint8_t *)buf, len, 0xFFFF);
    }   
}

/*硬实时任务：时间精度要求在微秒级（如严格 1ms/5ms），周期一旦抖动，控制算法直接失效。，严禁在轮询中排队。-->. 前台（中断）：接管微秒级“硬实时”*/

