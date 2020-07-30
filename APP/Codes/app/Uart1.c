/*
 * @Author: your name
 * @Date: 2020-05-29 09:49:07
 * @LastEditTime: 2020-07-30 18:09:14
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \APP\Codes\app\Uart1.c
 */
#include "osal.h"
//#include "osif_freertos.h"
#include "app.h"
#include "sys.h"
#include "Os.h"

#include "EventDefine.h"
#include "cychdr.h"
#include "usart.h"
#include "MsgDefine.h"

s32 SendMsgToUart1Manage(u8 *MsgBuff, u16 MsgLen)
{
    u32 ret = 0;

    ret = OS_MQSend(TASK_NAME(Uart1), MsgBuff, MsgLen);
    return ret;
}

void Uart1_MsgHandle(void *pbuf, u16 buflen)
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
            CommCmdDataCoderAndSend(pMsg_t);
            break;
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

/**
 * @description: 子任务主函数
 * @param {type} 
 * @return: 
 */
TASK_MAIN(Uart1)
{
}

/**
 * @description: 子任务接收其他任务消息处理函数
 * @param {type} 
 * @return: 
 */
TASK_MSG_HANDLE(Uart1)
{
    /*从邮箱去取数据*/

    u8 tskId;
    u16 BufLen;
    u8 *pBuf = NULL;

    tskId = xTaskGetCurrentTaskHandle();

    if (0 == OS_ReceiveMailbox(tskId, &pBuf, &BufLen, OS_DONOTWAIT))
    {
        if (tskId == TID_Uart1)
        {
            Uart1_MsgHandle(pBuf, BufLen);
        }

        OS_FreeMailBoxMemory(pBuf);
    }
}

TASK_INIT(Uart1)
{

    uart1_init(Uart1_TxIntHandle, Uart1_RxIntHandle);

    //enableCycleEvent((TASK_NAME(Uart1)));
}

TASK_CREATE(Uart1)
{
    cfThreadCreate(TASK_NAME(Uart1), Uart1Manage_Func, Uart1Manage_MsgHandle);
}

/* TASK Function */
TASK(Uart1)
{
    /*
    先初始化 , 在循环
    */
    TASK_INIT_Uart1();

    EvtBits_t EvtBit;

    //SwcGroupHandleInitFunction(&SwcsOfCanTask);
    //MessageReadyListInit(&TaskMsgReadyList[TASK_NAME(UartTask)][0], MESSAGE_LIST_SIZE(COM_RECEIVE_MESSAGE_COUNT, 32));

    for (;;)
    {
        EvtBit = EvtGroupsWaitBits(TASK_NAME(Uart),
                                   (EVENT_GLOBAL_MAILBOX | EVENT_GLOBAL_Watchdog |
                                    EVT_ID_All_Task_TIMER_5MS));

        if (EvtBit & EVENT_GLOBAL_MAILBOX)
        {
            //SwcGroupHandleMessage(&SwcsOfCanTask, TASK_NAME(UartTask));
            TASK_MSG_HANDLE_Uart1();
        }
        else if (EvtBit & EVT_ID_All_Task_TIMER_5MS)
        {
            //SwcGroupHandleMainFunction(&SwcsOfCanTask, 5);
            TASK_MAIN_Uart1();
        }
        else
        {
            //SwcGroupHandleEvent(&SwcsOfCanTask, EvtBit);
        }
    }
}