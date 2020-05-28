#include "GpioCfg.h"

/*------------ Initializes the GPIO sequence table --------------*/

const GPIO_InitTypeDef Gpio_InitConfigArr[NUM_OF_CONFIGURED_PINS] =
{
    {
         /*IO_WK_UP */
        .base = GPIOA,
        .Pin = 0,
        .Pull = GPIO_PULLDOWN,
        .Mode = GPIO_MODE_INPUT,
        .Speed = GPIO_SPEED_HIGH,
        .Alternate = 0,
    },
    {
         /*IO_USART1_TX */
        .base = GPIOA,
        .Pin = 9,
        .Pull = GPIO_PULLUP,
        .Mode = GPIO_MODE_AF_PP,
        .Speed = GPIO_SPEED_FAST,
        .Alternate = 7,
    },
    {
         /*IO_USART1_RX */
        .base = GPIOA,
        .Pin = 10,
        .Pull = GPIO_PULLUP,
        .Mode = GPIO_MODE_AF_PP,
        .Speed = GPIO_SPEED_FAST,
        .Alternate = 7,
    },
    {
         /*IO_LED1 */
        .base = GPIOB,
        .Pin = 0,
        .Pull = GPIO_PULLUP,
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Speed = GPIO_SPEED_HIGH,
        .Alternate = 0,
    },
    {
         /*IO_LED0 */
        .base = GPIOB,
        .Pin = 1,
        .Pull = GPIO_PULLUP,
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Speed = GPIO_SPEED_HIGH,
        .Alternate = 0,
    },
    {
         /*IO_KEY2 */
        .base = GPIOC,
        .Pin = 13,
        .Pull = GPIO_PULLUP,
        .Mode = GPIO_MODE_INPUT,
        .Speed = GPIO_SPEED_HIGH,
        .Alternate = 0,
    },
    {
         /*IO_KEY1 */
        .base = GPIOH,
        .Pin = 2,
        .Pull = GPIO_PULLUP,
        .Mode = GPIO_MODE_INPUT,
        .Speed = GPIO_SPEED_HIGH,
        .Alternate = 0,
    },
    {
         /*IO_KEY0 */
        .base = GPIOH,
        .Pin = 3,
        .Pull = GPIO_PULLUP,
        .Mode = GPIO_MODE_INPUT,
        .Speed = GPIO_SPEED_HIGH,
        .Alternate = 0,
    },
    {
         /*IO_IIC_SCL */
        .base = GPIOH,
        .Pin = 4,
        .Pull = GPIO_PULLUP,
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Speed = GPIO_SPEED_FAST,
        .Alternate = 4,
    },
    {
         /*IO_IIC_SDA */
        .base = GPIOH,
        .Pin = 5,
        .Pull = GPIO_PULLUP,
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Speed = GPIO_SPEED_FAST,
        .Alternate = 4,
    },
};

const PinNameSt IoTable[]  = {
    {GPIOA, 0},		/* IO_WK_UP */
    {GPIOA, 9},		/* IO_USART1_TX */
    {GPIOA, 10},		/* IO_USART1_RX */
    {GPIOB, 0},		/* IO_LED1 */
    {GPIOB, 1},		/* IO_LED0 */
    {GPIOC, 13},		/* IO_KEY2 */
    {GPIOH, 2},		/* IO_KEY1 */
    {GPIOH, 3},		/* IO_KEY0 */
    {GPIOH, 4},		/* IO_IIC_SCL */
    {GPIOH, 5},		/* IO_IIC_SDA */
};

