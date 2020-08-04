/*
 * @Author: your name
 * @Date: 2020-08-03 17:46:21
 * @LastEditTime: 2020-08-03 18:04:18
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \J0001-RTOS\APP\Codes\app\Lcd.c
 */
#include "Lcd.h"
#include "ResourceConfig.h"
#include "MsgDefine.h"
#include "Os.h"
#include "cychdr.h"


u32 SendMsgToLcd(u8 *MsgData, u16 MsgLen)
{
    u32 ret;
    ret = OS_MQSend(TASK_NAME(Lcd), MsgData, MsgLen);
    return ret;
}

static void Lcd_MsgHandle(void *pbuf, u16 buflen)
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
            //LedRecKeyMsgHandle(pMsg_t);
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
TASK_MSG_HANDLE(Lcd)
{
    /*从邮箱去取数据*/

    u16 BufLen;
    u8 *pBuf = NULL;
    //TaskHandle_t xHandle = NULL;

    if (TaskList[TID_Lcd] == xTaskGetCurrentTaskHandle())
    {
        if (0 == OS_ReceiveMailbox(TID_Lcd, &pBuf, &BufLen, OS_DONOTWAIT))
        {
            Lcd_MsgHandle(pBuf, BufLen);
            OS_FreeMailBoxMemory(pBuf);
        }
    }
}
/**
 * @description: 子任务主函数
 * @param {type} 
 * @return: 
 */
TASK_MAIN(Lcd)
{
    u32 EvtBit;

    for (;;)
    {
        EvtBit = EvtGroupsWaitBits(TASK_NAME(Lcd),
                                   (EVENT_GLOBAL_MAILBOX | EVENT_GLOBAL_Watchdog));

        if (EvtBit & EVENT_GLOBAL_MAILBOX)
        {
            TASK_MSG_HANDLE_Lcd();
        }
        else if (EvtBit & EVENT_GLOBAL_Watchdog)
        {
        }
        else
        {
        }
    }
}

TASK_INIT(Lcd)
{


    enableCycleEvent(TASK_NAME(Lcd));
}

/* TASK Function */
TASK(Lcd)
{
    /*先初始化 , 在循环*/
    TASK_INIT_Lcd();

    TASK_MAIN_Lcd();
}
