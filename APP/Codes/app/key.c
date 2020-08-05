#include "key.h"
#include "led.h"

#include "cychdr.h"
#include "MsgDefine.h"


u32 SendMsgToKey(u8 *MsgData, u16 MsgLen)
{
    u32 ret;
    ret = OS_MQSend(TASK_NAME(Key), MsgData, MsgLen);
    return ret;
}

/**
 * @description: 
 * @param {type} 
 * @return: 
 */
static void Key_ManageProc(void)
{
    KeyData_t KeyMsg;

    if (Key0)
    {
        KeyMsg.Msg.Type = EVENT_KEY_STATUS;
        KeyMsg.Key0_status = 1;
        SendMsgToLed((u8 *)&KeyMsg, sizeof(KeyData_t));
    }
    if (Key1)
    {
        KeyMsg.Msg.Type = EVENT_KEY_STATUS;
        KeyMsg.Key1_status = 1;
        SendMsgToLed((u8 *)&KeyMsg, sizeof(KeyData_t));
    }
    if (Key2)
    {
        KeyMsg.Msg.Type = EVENT_KEY_STATUS;
        KeyMsg.Key2_status = 1;
        SendMsgToLed((u8 *)&KeyMsg, sizeof(KeyData_t));
    }
    if (Key_WK_UP)
    {
        KeyMsg.Msg.Type = EVENT_KEY_STATUS;
        KeyMsg.Key_WK_UP_status = 1;
        SendMsgToLed((u8 *)&KeyMsg, sizeof(KeyData_t));
    }
}

static void Key_MsgHandle(void *pbuf, u16 buflen)
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
TASK_MSG_HANDLE(Key)
{
    /*从邮箱去取数据*/

    u16 BufLen;
    u8 *pBuf = NULL;
    //TaskHandle_t xHandle = NULL;

    if (TaskList[TID_Key] == xTaskGetCurrentTaskHandle())
    {
        if (0 == OS_ReceiveMailbox(TID_Key, &pBuf, &BufLen, OS_DONOTWAIT))
        {
            Key_MsgHandle(pBuf, BufLen);
            OS_FreeMailBoxMemory(pBuf);
        }
    }
}
/**
 * @description: 子任务主函数
 * @param {type} 
 * @return: 
 */
TASK_MAIN(Key)
{
    u32 EvtBit;

    for (;;)
    {
        EvtBit = EvtGroupsWaitBits(TASK_NAME(Key),
                                   (EVENT_GLOBAL_MAILBOX | EVENT_GLOBAL_Watchdog |
                                    EVENT_GLOBAL_TIMER_10MS));

        if (EvtBit & EVENT_GLOBAL_MAILBOX)
        {
            TASK_MSG_HANDLE_Key();
        }
        else if (EvtBit & EVENT_GLOBAL_TIMER_10MS)
        {
            Key_ManageProc();
        }
        else if (EvtBit & EVENT_GLOBAL_Watchdog)
        {
        }
        else
        {
        }
    }
}

TASK_INIT(Key)
{

    enableCycleEvent(TASK_NAME(Key));
}

/* TASK Function */
TASK(Key)
{
    /*先初始化 , 在循环*/
    TASK_INIT_Key();

    TASK_MAIN_Key();
}
