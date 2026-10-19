#ifndef __BSP_DS18B20_H
#define __BSP_DS18B20_H
/*黄金法则：.h 文件里只放“为了让 .h 自己的声明不报错”所必需的最少头文件（比如用到了 uint8_t 就只包含 <stdint.h>）*/
#include <stdint.h>

typedef enum {
    DS18B20_OK = 0,
    DS18B20_ERR_OFFLINE // 传感器未连接或损坏
} ds18b20_status_t;


ds18b20_status_t BSP_DS18B20_Init(void);
float BSP_DS18B20_ReadTemp(void);

#endif