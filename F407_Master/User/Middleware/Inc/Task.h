#ifndef __TASK_H
#define __TASK_H

#include <stdint.h>
extern uint32_t HAL_GetTick(void); // 声明外部时间函数，避免直接包含庞大的 hal.h

/*适用于无返回值函数内部*/
#define PERIODIC(NAME, T) \
    static uint32_t NAME##_last = 0; /* 避开外设刚上电不稳定时期 */ \
    uint32_t NAME##_now = HAL_GetTick(); \
    if ((uint32_t)(NAME##_now - NAME##_last) < (uint32_t)(T)) return; \
    NAME##_last = NAME##_now

/*适用于代码块/有返回值函数内部*/
#define PERIODIC_START(NAME, T) \
    do { \
        static uint32_t NAME##_last = 0; \
        uint32_t NAME##_now = HAL_GetTick(); \
        if ((uint32_t)(NAME##_now - NAME##_last) >= (uint32_t)(T)) { \
            NAME##_last = NAME##_now;
#define PERIODIC_END \
        } \
    } while (0)

#endif /* __TASK_H */