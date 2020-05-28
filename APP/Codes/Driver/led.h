#ifndef _LED_H
#define _LED_H

#include "GPIO.h"

#define LED0 PBout(1)   //LED0
#define LED1 PBout(0)   //LED1

void LED_Init(void);
void Set_Led0(u8 OnOFF);
void Set_Led1(u8 OnOFF);


#endif
