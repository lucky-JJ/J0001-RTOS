#include "osal.h"
#include "osif_freertos.h"
#include "app.h"
#include "sys.h"
#include "EventDefine.h"



s32 SendMsgToUart1Manage(u8 *MsgData, u16 MsgLen)
{
    u32 ret = 0;


    ret = OSIF_MQSend(TASK_NAME(Uart1), MsgData, MsgLen);
    return ret;
}


void Uart1Manage_MsgHandle(void *pbuf, u16 buflen)
{
	/* 消息内容都是从邮箱取的 */

}


void Uart1Manage_Func(void)
{
	u32 tSignals;


	/* wait for events indefinitely and clear after end of function */
	tSignals = cfThreadWaitWithTimeout((EVENT_GLOBAL_Watchdog |
										EVENT_Uart1_TIMER_5MS_EVENT),
										OS_WAITFOREVER);


	if(tSignals & EVENT_GLOBAL_Watchdog)
	{
		;
	}

}


TASK_INIT(Uart1)
{
	//SetAlarm(ALARM_NAME(PwrManage), 5, 10);

	enableCycleEvent((TASK_NAME(Uart1)));

}

TASK_CREATE(Uart1)
{
    cfThreadCreate(TASK_NAME(Uart1), Uart1Manage_Func, Uart1Manage_MsgHandle);
}





