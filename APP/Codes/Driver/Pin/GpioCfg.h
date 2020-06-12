#ifndef GPIO_CFG_H
#define GPIO_CFG_H

#include "sys.h"

#define  NUM_OF_CONFIGURED_PINS      (14)

typedef struct
{
    GPIO_TypeDef *			base;              /*!< Port base pointer.     */
    u32						pinPortIdx;        /*!< Port pin number.       */
} PinNameSt;

typedef enum {
    IO_WK_UP,
    IO_USART1_TX,
    IO_USART1_RX,
    IO_LED1,
    IO_LED0,
    IO_KEY2,
    IO_F_CS,
    IO_SPI5_SCK,
    IO_SPI5_MISO,
    IO_SPI5_MOSI,
    IO_KEY1,
    IO_KEY0,
    IO_IIC_SCL,
    IO_IIC_SDA,
    MAX_IO_NUM,
}Io_Type;

extern const GPIO_InitTypeDef Gpio_InitConfigArr[NUM_OF_CONFIGURED_PINS];
extern const GPIO_InitTypeDef Gpio_DeInitConfigArr[NUM_OF_CONFIGURED_PINS];
extern const PinNameSt IoTable[];


#endif
