#ifndef GPIO_H
#define GPIO_H

#include "GpioCfg.h"

u8 GPIO_Read(Io_Type SignalType);
void GPIO_Write(Io_Type SignalType, GPIO_PinState PinState);
void Gpio_Init(const GPIO_InitTypeDef *config);

#endif
