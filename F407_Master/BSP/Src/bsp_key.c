#include "bsp_key.h"
#include "stm32f4xx_hal.h"

/*结构体查表法*/ 
typedef struct {
    GPIO_TypeDef *port;       
    uint16_t      pin;        
    uint8_t       is_pressed; // 记录上一次扫描时，按键是不是已经按下了 (0:未按下, 1:已按下)
    uint32_t      press_time; // 记录刚按下那一瞬间的 HAL_GetTick() 时间
} key_ctrl_t;

/* 无const结构体里的 is_pressed 和 press_time 可以随时修改，const适合用于定义只读配置比如LED引脚 */
static key_ctrl_t key_configs[KEY_ID_NUM] = {
    [KEY_ID_0] = {GPIOE, GPIO_PIN_2, 0, 0},
    [KEY_ID_1] = {GPIOE, GPIO_PIN_3, 0, 0},     
    [KEY_ID_2] = {GPIOE, GPIO_PIN_4, 0, 0},
};

/*
  * @brief  扫描按键状态函数，核心一件事按下计时，松手结算。该函数需要被周期性调用
  * FreeRtos任务/定时器中断/主循环轮询（任务较少且资源充足）
  * @param  id 按键编号
  * @retval 按键状态：KEY_NONE, KEY_SHORT, KEY_LONG
  */
key_state_t BSP_KEY_Scan(key_id_t id)
{
    if (id >= KEY_ID_NUM) return KEY_NONE;

    key_ctrl_t *cfg = &key_configs[id]; 

    /* 按键被按下*/
    if (HAL_GPIO_ReadPin(cfg->port, cfg->pin) == GPIO_PIN_RESET) 
    {
        if (cfg->is_pressed == 0) // 说明之前没按下，现在是“刚按下的第一瞬间”！
        {
            cfg->is_pressed = 1;               // 标记为已按下
            cfg->press_time = HAL_GetTick();   // 记下按下这一刻的时间（只记一次！）
        }
        return KEY_NONE; // 按住没松手，先不触发事件，等松手再结算
    }
    /*按键处于松开状态*/
    else 
    {
        if (cfg->is_pressed == 1) // 说明之前按下了，现在是“刚刚松手的那一瞬间”！
        {
            cfg->is_pressed = 0; // 恢复为未按下状态
    
            uint32_t hold_time = HAL_GetTick() - cfg->press_time;// 计算从按下到松手一共经历了多少毫秒

            if (hold_time >= 2000) return KEY_LONG;  // 长按！
        
            else if (hold_time >= 20) return KEY_SHORT; // 短按！
        }
    }
    return KEY_NONE; // 平时没按的时候，一直返回 KEY_NONE
}