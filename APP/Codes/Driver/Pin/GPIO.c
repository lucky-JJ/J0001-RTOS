/*
 * @Author: your name
 * @Date: 2020-05-29 09:49:07
 * @LastEditTime: 2020-07-29 09:09:27
 * @LastEditors: your name
 * @Description: In User Settings Edit
 * @FilePath: \APP\Codes\Driver\Pin\GPIO.c
 */
#include "GPIO.h"

u8 GPIO_Read(Io_Type SignalType)
{
    return (u8)HAL_GPIO_ReadPin((GPIO_TypeDef *)IoTable[SignalType].base, (u16)IoTable[SignalType].pinPortIdx);
}

void GPIO_Write(Io_Type SignalType, GPIO_PinState PinState)
{
    if (SignalType < MAX_IO_NUM)
    {
        HAL_GPIO_WritePin(IoTable[SignalType].base, (u16)IoTable[SignalType].pinPortIdx, PinState);
    }
}

void Gpio_Init(const GPIO_InitTypeDef *config)
{
    u32 i;
    GPIO_InitTypeDef GPIO_Initure;

    /* STM32F4xx  GPIO_InitTypeDef need be add (GPIO_TypeDef * base) to adapt the script*/

    /* The clock must be enabled first */

    for (i = 0; i < NUM_OF_CONFIGURED_PINS; i++)
    {
        GPIO_Initure.base = config[i].base;
        GPIO_Initure.Pin = config[i].Pin;
        GPIO_Initure.Mode = config[i].Mode;
        GPIO_Initure.Pull = config[i].Pull;
        GPIO_Initure.Speed = config[i].Speed;
        GPIO_Initure.Alternate = config[i].Alternate;

        HAL_GPIO_Init(GPIO_Initure.base, &GPIO_Initure);
    }
}
