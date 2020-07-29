/*
 * @Author: your name
 * @Date: 2020-05-29 09:49:07
 * @LastEditTime: 2020-07-29 09:56:46
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \APP\Codes\System\Stattup\main.c
 */

#include "sys.h"
#include "delay.h"
#include "usart.h"
//#include "led.h"
#include "FreeRTOS.h"
#include "task.h"

//#include "osal.h"
#include "app.h"

#include "GPIO.h"
#include "cychdr.h"

#include "Os_Init"
#include "Irq.h"

/*
	设计3个任务 .
	1.key 按键按下检测,通知task2
	2.收到按键按下消息 , 点亮/熄灭led, 通知task3
	3.收到消息通过串口打印输出

    Task1 : key
    Task2 : led
    Task3 : uart/debugAndshell
*/
int main(void)
{
    HAL_Init();

    SystemClock_Init();

    Gpio_Init(&Gpio_InitConfigArr[0]);

    // Initialize the OS
    InitOS();
    // Setup interrupts
    IRQ_Init();

    //Bsp_Init();

    //I2C
    //SPI
    //FLASH
    //ADC
    //PWM
    //RTC
    //debugandshell
    //watchdog

    //cycleTaskInit();

    //cfThreadInit();

    StartOS();

    while (1)
        return 0;
}
