#ifndef __BSP_ONE_WIRE_H
#define __BSP_ONE_WIRE_H

#include <stdint.h>

//选择自己喜欢的方式
#define REG 1

#if REG
/*对于168MHZ的系统时钟来说函数压栈，执行大概需要10~20个周期也就是60~120us对于OW通信ms级别来说可以忽略*/
#define OW_LOW()          HAL_GPIO_WritePin(GPIOG, GPIO_PIN_15, GPIO_PIN_RESET)
#define OW_RELEASE()      HAL_GPIO_WritePin(GPIOG, GPIO_PIN_15, GPIO_PIN_SET)
#define OW_READ()         HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_15)
#else
/* 直接寄存器操作，等效于 HAL 函数 */
#define OW_LOW()       (GPIOG->BSRR = (uint32_t)GPIO_PIN_15 << 16U)  
#define OW_RELEASE()   (GPIOG->BSRR = GPIO_PIN_15)                    
#define OW_READ()      ((GPIOG->IDR & GPIO_PIN_15) ? 1U : 0U)         
#endif

uint8_t One_Wire_Reset_And_Check(void);
void One_Wire_Write_Byte(uint8_t data);
uint8_t One_Wire_Read_Byte(void);

#endif