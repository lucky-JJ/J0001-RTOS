#include "osal.h"
#include "osif_freertos.h"
#include "app.h"
#include "sys.h"
#include "EventDefine.h"



s32 SendMsgToLedManage(u8 *MsgData, u16 MsgLen)
{
    u32 ret = 0;


    ret = OSIF_MQSend(TASK_NAME(Led), MsgData, MsgLen);
    return ret;
}


void LedManage_MsgHandle(void *pbuf, u16 buflen)
{

}


void LedManage_Func(void)
{
	u32 tSignals;


	/* wait for events indefinitely and clear after end of function */
	tSignals = cfThreadWaitWithTimeout((EVENT_GLOBAL_Watchdog |
										EVENT_Led_TIMER_5MS_EVENT |
										EVENT_Led_TIMER_20MS_EVENT ),
										OS_WAITFOREVER);


	if(tSignals & EVENT_GLOBAL_Watchdog)
	{
		;
	}

}


TASK_INIT(Led)
{


	enableCycleEvent((TASK_NAME(Led)));

}

TASK_CREATE(Led)
{
    cfThreadCreate(TASK_NAME(Led), LedManage_Func, LedManage_MsgHandle);
}





