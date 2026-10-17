#ifndef __TASK_H
#define __TASK_H

#define PERIODIC(NAME, T) \
    static uint32_t NAME##_last = 0; \
    uint32_t NAME##_now = HAL_GetTick(); \
    if ((uint32_t)(NAME##_now - NAME##_last) < (uint32_t)(T)) return; \
    NAME##_last = NAME##_now


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