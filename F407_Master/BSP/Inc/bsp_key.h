#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#include <stdint.h>

typedef enum {
    KEY_ID_0 = 0,
    KEY_ID_1,
    KEY_ID_2,
    KEY_ID_NUM
} key_id_t;

typedef enum {
    KEY_NONE = 0,
    KEY_SHORT,
    KEY_LONG
} key_state_t;

key_state_t BSP_KEY_Scan(key_id_t id); // 扫描并返回当前按键结果

#endif