#include"bsp_one_wire.h"
#include "Delay_us.h"
#include "stm32f4xx_hal_gpio.h"
#include <stdint.h>


/*复位 + 存在检测*/
uint8_t One_Wire_Reset_And_Check(void)
{
    __disable_irq();          
    OW_LOW();
    Delay_us(480);
    OW_RELEASE();
    Delay_us(70);
    uint8_t ack =!OW_READ();
    Delay_us(410);
    __enable_irq();  
    return ack;
}

static void One_Wire_Write_Bit(uint8_t bit)
{
    __disable_irq(); 
    OW_LOW();
    if(bit)
    {
        Delay_us(1);
        OW_RELEASE();
        Delay_us(60);
    }
    else
    {
        Delay_us(60);
        OW_RELEASE();
    }

    Delay_us(2);//位间隙
    __enable_irq();
}

static uint8_t One_Wire_Read_Bit(void)
{
    __disable_irq(); 
    OW_LOW();
    Delay_us(1);
    OW_RELEASE();

    Delay_us(10);
    uint8_t aa=OW_READ();

    Delay_us(60);

    __enable_irq();//在返回前开启中断
    return aa;
}

/*写 1 个字节*/
void One_Wire_Write_Byte(uint8_t data)
{
    for(int i=0;i<8;i++)
    {
        if((data>>i)&0x01)
        One_Wire_Write_Bit(1);
        else
        One_Wire_Write_Bit(0);
    }
}

/*读 1 个字节*/
uint8_t One_Wire_Read_Byte(void)
{
    uint8_t data = 0;
    for (int i = 0; i < 8; i++)
        data |= (uint8_t)(One_Wire_Read_Bit() << i);  /* LSB 先发，拼到第 i 位 */

    return data;
}

