#ifndef __BSP_LED_H
#define __BSP_LED_H

#include <stdint.h>

/* 枚举 LED 编号 */
typedef enum {
    LED_RED = 0,
    LED_GREEN,
    LED_MAX_NUM
} LED_Id_t;

/* 枚举 LED 期望动作 */
typedef enum {
    LED_ON=0,
    LED_OFF,
    LED_TOGGLE
} LED_State_t;
    
/* 对外暴露的通用 API */
void BSP_LED_Init(void);
void BSP_LED_Set(LED_Id_t led, LED_State_t state);

/* 便捷操作接口，根据自己需要拓展彩灯API */
void BSP_LED_On(LED_Id_t led);
void BSP_LED_Off(LED_Id_t led);
void BSP_LED_Toggle(LED_Id_t led);

#endif /* __BSP_LED_H */