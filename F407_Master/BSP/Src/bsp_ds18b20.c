#include "bsp_ds18b20.h"
#include"bsp_one_wire.h"
#include"stm32f4xx_hal.h"
#include "stm32f4xx_hal_cortex.h"
#include <stdint.h>

/*这些宏定义是模块私有，为减少代码耦合性不能放在.h文件！*/
#define DS18B20_CONVERT_TIME_MS   750U//温度转换一次所需时间大概750ms
#define Skip_ROM	0xCC	//跳过 64 位 ROM 地址匹配。总线上只有一个 DS18B20 时，用它最方便
#define Convert_T	0x44	//启动一次温度转换
#define Read_Scratchpad	0xBE	//读取暂存器，温度值就在前两个字节


ds18b20_status_t BSP_DS18B20_Init(void)
{
    if(One_Wire_Reset_And_Check()==0)
    return DS18B20_ERR_OFFLINE;

    /* 初始化时，先提前触发第一次温度转换，然后直接走人，不死等！*/
    One_Wire_Write_Byte(Skip_ROM);
    One_Wire_Write_Byte(Convert_T);

    return DS18B20_OK;
}

/* 
 * 非阻塞读取函数：执行时间不到 2ms，绝不卡死 CPU！
 */
float BSP_DS18B20_ReadTemp(void)
{
    static uint32_t last_time = 0;
    static float last_temp = 25.0f;

    /* 转换时间未到，直接返回上一次温度 */
    if (HAL_GetTick() - last_time < DS18B20_CONVERT_TIME_MS)
        return last_temp;

    /* 1. 复位并检测存在脉冲：返回 0 说明离线！ */
    if (One_Wire_Reset_And_Check() == 0)
        return -999.999f;

    One_Wire_Write_Byte(Skip_ROM);   
    One_Wire_Write_Byte(Read_Scratchpad);   

    uint8_t lsb = One_Wire_Read_Byte();  // 温度低字节
    uint8_t msb = One_Wire_Read_Byte();  // 温度高字节

    /* 2. 如果读回来全是 0xFF（总线悬空）或特定错误值 */
    if (lsb == 0xFF && msb == 0xFF)
        return -999.999f; // 数据全异常

    int16_t raw = (int16_t)(((uint16_t)msb << 8) | lsb);

    /* 默认 12 位分辨率，1 LSB = 0.0625°C */
    last_temp = (float)raw * 0.0625f;

    /* 3. 启动下一次温度转换：返回 1（在线）才发送转换指令 */
    if (One_Wire_Reset_And_Check() == 1)
    {
        One_Wire_Write_Byte(Skip_ROM);
        One_Wire_Write_Byte(Convert_T);
        last_time = HAL_GetTick();
    }

    return last_temp;
}

