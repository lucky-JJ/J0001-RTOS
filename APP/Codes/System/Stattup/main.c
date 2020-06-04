
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"

#include "osal.h"
#include "app.h"

#include "GPIO.h"
#include "cychdr.h"


/*
	设计3个任务 .
	1.key 按键按下检测,通知task2
	2.收到按键按下消息 , 点亮/熄灭led, 通知task3
	3.收到消息通过串口打印输出

*/
int main(void)
{
    HAL_Init();                     //初始化HAL库
    Stm32_Clock_Init(360,25,2,8);   //设置时钟,180Mhz

	Gpio_Init(&Gpio_InitConfigArr[0]);

	delay_init(180);                //初始化延时函数
    LED_Init();                     //初始化LED
    


	//Bsp_Init();

	//I2C
	//SPI
	//FLASH
	//ADC
	//PWM
	//RTC
	//debugandshell
	//watchdog

	cycleTaskInit();

    cfThreadInit();

	vTaskStartScheduler();

	while(1);

}



