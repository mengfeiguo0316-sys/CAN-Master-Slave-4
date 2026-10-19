#include"Delay_us.h"

/**
 * @brief  初始化 DWT 计数器
 * @note   在 main() 中系统时钟配置完成后调用一次
 */
void Delay_Init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;  /* 使能 DWT 外设 */
    DWT->CYCCNT = 0;                                  /* 计数器清零 */
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;              /* 启动计数器 */
}

/**
 * @brief  微秒级延时（阻塞式）
 * @param  us  延时微秒数
 */
void Delay_us(uint32_t us)
{
    uint32_t start  = DWT->CYCCNT;
    uint32_t ticks  = us * (SystemCoreClock / 1000000U);  /* 换算成时钟周期数 */
    while ((DWT->CYCCNT - start) < ticks)
    {
        /* 空转等待 */
    }
}
