/*
 * @Author: your name
 * @Date: 2020-05-29 09:49:07
 * @LastEditTime: 2020-08-04 09:35:18
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \APP\Codes\app\Uart1.c
 */
//#include "osal.h"
//#include "osif_freertos.h"
//#include "app.h"
//#include "sys.h"
#include "Os.h"

//#include "EventDefine.h"
#include "cychdr.h"
#include "usart.h"
#include "MsgDefine.h"
#include "ResourceConfig.h"

#include "usart.h"

s32 SendMsgToUart1(u8 *MsgBuff, u16 MsgLen)
{
    u32 ret = 0;

    ret = OS_MQSend(TASK_NAME(Uart1), MsgBuff, MsgLen);
    return ret;
}

static void Uart1_MsgHandle(void *pbuf, u16 buflen)
{
    /* 消息内容都是从邮箱取的 */

    Message_t *pMsg_t;

    pMsg_t = (Message_t *)pbuf;

    if (pMsg_t != 0)
    {
        switch (pMsg_t->Type)
        {
        case EVENT_SEND_DATA_TO_UART1:
        {
            //CommCmdDataCoderAndSend(pMsg_t);
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
TASK_MSG_HANDLE(Uart1)
{
    /*从邮箱去取数据*/

    u16 BufLen;
    u8 *pBuf = NULL;
    //TaskHandle_t xHandle = NULL;

    if (TaskList[TID_Uart1] == xTaskGetCurrentTaskHandle())
    {
        if (0 == OS_ReceiveMailbox(TID_Uart1, &pBuf, &BufLen, OS_DONOTWAIT))
        {
            Uart1_MsgHandle(pBuf, BufLen);
            OS_FreeMailBoxMemory(pBuf);
        }
    }
}

/**
 * @description: 子任务主函数
 * @param {type} 
 * @return: 
 */
TASK_MAIN(Uart1)
{
    EvtBits_t EvtBit;

    for (;;)
    {
        EvtBit = EvtGroupsWaitBits(TASK_NAME(Uart1),
                                   (EVENT_GLOBAL_MAILBOX | EVENT_GLOBAL_Watchdog |
                                    EVENT_GLOBAL_TIMER_5MS | EVENT_GLOBAL_TIMER_10MS));

        if (EvtBit & EVENT_GLOBAL_MAILBOX)
        {
            TASK_MSG_HANDLE_Uart1();
        }
        else if (EvtBit & EVENT_GLOBAL_TIMER_5MS)
        {
            USART1_ManageProc(5);
        }
        else if (EvtBit & EVENT_GLOBAL_TIMER_10MS)
        {
        }
        else if (EvtBit & EVENT_GLOBAL_Watchdog)
        {
        }
        else
        {
        }
    }
}

TASK_INIT(Uart1)
{

    uart1_init(Uart1_TxIntHandle, Uart1_RxIntHandle);

    enableCycleEvent(TASK_NAME(Uart1));
}

/* TASK Function */
TASK(Uart1)
{
    /*先初始化 , 在循环*/
    TASK_INIT_Uart1();

    TASK_MAIN_Uart1();
}
