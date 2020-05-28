#include "led.h"
#include "sys.h"


void LED_Init(void)
{
	Set_Led0(ON);
	Set_Led1(ON);
}

void Set_Led0(u8 OnOFF)
{
	if(OnOFF == ON)
	{
		GPIO_Write(IO_LED0, GPIO_PIN_SET);
	}
	else
	{
		GPIO_Write(IO_LED0, GPIO_PIN_RESET);
	}
}

void Set_Led1(u8 OnOFF)
{
	if(OnOFF == ON)
	{
		GPIO_Write(IO_LED1, GPIO_PIN_SET);
	}
	else
	{
		GPIO_Write(IO_LED1, GPIO_PIN_RESET);
	}
}

