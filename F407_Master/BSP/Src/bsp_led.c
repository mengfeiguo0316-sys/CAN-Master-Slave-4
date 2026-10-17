#include "bsp_led.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"

void BSP_LED_Set(LED_Id_t led, LED_State_t state);


/* 定义单个 LED 的硬件特性结构体 */
typedef struct {
    GPIO_TypeDef    *port;
    uint16_t        pin;
    GPIO_PinState   active_level;
} led_hw_config_t;

/* 硬件配置表：迁移时，只需修改这张表里的端口、引脚或电平极性 */
static const led_hw_config_t g_led_configs[LED_MAX_NUM] = {
    [LED_RED]   = {GPIOE, GPIO_PIN_0, GPIO_PIN_RESET},
    [LED_GREEN] = {GPIOE, GPIO_PIN_1, GPIO_PIN_RESET},
};

void BSP_LED_Init(void)
{
    for (uint8_t i = 0; i < LED_MAX_NUM; i++)
        BSP_LED_Off((LED_Id_t)i);
}

void BSP_LED_Set(LED_Id_t led, LED_State_t state)
{
    if (led >= LED_MAX_NUM)
        return;

    const led_hw_config_t *cfg = &g_led_configs[led];
    GPIO_PinState inactive_level = (cfg->active_level == GPIO_PIN_SET) ? GPIO_PIN_RESET : GPIO_PIN_SET;//确定熄灭电平

    switch (state)
    {
        case LED_ON:
            HAL_GPIO_WritePin(cfg->port, cfg->pin, cfg->active_level);
            break;
        case LED_OFF:
            HAL_GPIO_WritePin(cfg->port, cfg->pin, inactive_level);
            break;
        case LED_TOGGLE:
            HAL_GPIO_TogglePin(cfg->port, cfg->pin);
            break;
        default:
            break;
    }
}

void BSP_LED_On(LED_Id_t led)     { BSP_LED_Set(led, LED_ON); }
void BSP_LED_Off(LED_Id_t led)    { BSP_LED_Set(led, LED_OFF); }
void BSP_LED_Toggle(LED_Id_t led) { BSP_LED_Set(led, LED_TOGGLE); }