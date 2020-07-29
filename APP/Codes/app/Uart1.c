/*
 * @Author: your name
 * @Date: 2020-05-29 09:49:07
 * @LastEditTime: 2020-07-29 13:53:07
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \APP\Codes\app\Uart1.c
 */
#include "osal.h"
#include "osif_freertos.h"
#include "app.h"

#include "EventDefine.h"
#include "cychdr.h"
#include "usart.h"
#include "MsgDefine.h"

s32 SendMsgToUart1Manage(u8 *MsgData, u16 MsgLen)
{
    u32 ret = 0;

    ret = OSIF_MQSend(TASK_NAME(Uart1), MsgData, MsgLen);
    return ret;
}

void Uart1Manage_MsgHandle(void *pbuf, u16 buflen)
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
            /*其他任务发消息过来这里,通过这里把数据用串口发出去*/
            CommCmdDataCoderAndSend(pMsg_t);
        }
        }
    }
}

void Uart1Manage_Func(void)
{
    u32 tSignals;

    /* wait for events indefinitely and clear after end of function */
    tSignals = cfThreadWaitWithTimeout((EVENT_GLOBAL_Watchdog |
                                        EVENT_Uart1_TIMER_5MS_EVENT),
                                       OS_WAITFOREVER);

    if (tSignals & EVENT_TIMER_5MS)
    {
        USART1_ManageProc(10);
    }
    if (tSignals & EVENT_Uart1_TX_FINISH)
    {
        USART1_SendDataManage();
    }
    else if (tSignals & EVENT_GLOBAL_Watchdog)
    {
        ;
    }
}

TASK_MAIN(Uart1)
{
}

TASK_MSG_HANDLE(Uart1)
{
}

TASK_INIT(Uart1)
{

    uart1_init(Uart1_TxIntHandle, Uart1_RxIntHandle);

    enableCycleEvent((TASK_NAME(Uart1)));
}

TASK_CREATE(Uart1)
{
    cfThreadCreate(TASK_NAME(Uart1), Uart1Manage_Func, Uart1Manage_MsgHandle);
}
