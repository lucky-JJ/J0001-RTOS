/*
 * @Author: your name
 * @Date: 2020-05-29 09:49:07
 * @LastEditTime: 2020-08-03 15:35:14
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \J0001-RTOS\APP\Codes\Driver\led.c
 */
#include "led.h"

#include "ResourceConfig.h"
#include "GPIO.h"
//#include "stm32f4xx_hal_gpio.h"
#include "MsgDefine.h"
#include "Os.h"
#include "key.h"

static void Set_Led0(u8 OnOFF)
{
    if (OnOFF == ON)
    {
        GPIO_Write(IO_LED0, GPIO_PIN_SET);
    }
    else
    {
        GPIO_Write(IO_LED0, GPIO_PIN_RESET);
    }
}

static void Set_Led1(u8 OnOFF)
{
    if (OnOFF == ON)
    {
        GPIO_Write(IO_LED1, GPIO_PIN_SET);
    }
    else
    {
        GPIO_Write(IO_LED1, GPIO_PIN_RESET);
    }
}
static void LedRecKeyMsgHandle(Message_t *pMsg)
{
    if (pMsg == NULL)
        return;

    KeyData_t *pEvt = (KeyData_t *)pMsg;

    if (pEvt->Key0_status == 1 || pEvt->Key1_status == 1 || pEvt->Key2_status == 1)
        Set_Led0(ON);

    if (pEvt->Key_WK_UP_status == 1)
        Set_Led0(OFF);
}

u32 SendMsgToLed(u8 *MsgData, u16 MsgLen)
{
    u32 ret;
    ret = OS_MQSend(TASK_NAME(Led), MsgData, MsgLen);
    return ret;
}

static void Led_MsgHandle(void *pbuf, u16 buflen)
{
    /* 消息内容都是从邮箱取的 */

    Message_t *pMsg_t;

    pMsg_t = (Message_t *)pbuf;

    if (pMsg_t != 0)
    {
        switch (pMsg_t->Type)
        {
        case EVENT_KEY_STATUS:
        {
            LedRecKeyMsgHandle(pMsg_t);
            break;
        }
        }
    }
}
/**
 * @description: 接收其他任务消息处理函数
 * @param {type} 
 * @return: 
 */
TASK_MSG_HANDLE(Led)
{
    /*从邮箱去取数据*/

    u16 BufLen;
    u8 *pBuf = NULL;
    //TaskHandle_t xHandle = NULL;

    if (TaskList[TID_Led] == xTaskGetCurrentTaskHandle())
    {
        if (0 == OS_ReceiveMailbox(TID_Led, &pBuf, &BufLen, OS_DONOTWAIT))
        {
            Led_MsgHandle(pBuf, BufLen);
            OS_FreeMailBoxMemory(pBuf);
        }
    }
}
/**
 * @description: 子任务主函数
 * @param {type} 
 * @return: 
 */
TASK_MAIN(Led)
{
    u32 EvtBit;

    for (;;)
    {
        EvtBit = EvtGroupsWaitBits(TASK_NAME(Led),
                                   (EVENT_GLOBAL_MAILBOX | EVENT_GLOBAL_Watchdog));

        if (EvtBit & EVENT_GLOBAL_MAILBOX)
        {
            TASK_MSG_HANDLE_Led();
        }
        else if (EvtBit & EVENT_GLOBAL_Watchdog)
        {
        }
        else
        {
        }
    }
}

TASK_INIT(Led)
{

    Set_Led0(ON);
    Set_Led1(ON);

    //enableCycleEvent(TASK_NAME(Led));
}

/* TASK Function */
TASK(Led)
{
    /*先初始化 , 在循环*/
    TASK_INIT_Led();

    TASK_MAIN_Led();
}
