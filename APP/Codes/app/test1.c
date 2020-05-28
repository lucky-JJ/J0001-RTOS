#include "osal.h"
#include "osif_freertos.h"
#include "app.h"
#include "sys.h"



s32 SendMsgToKeyManage(u8 *MsgData, u16 MsgLen)
{
    u32 ret = 0;


    ret = OSIF_MQSend(TASK_NAME(Key), MsgData, MsgLen);
    return ret;
}


void KeyManage_MsgHandle(void *pbuf, u16 buflen)
{

}


void KeyManage_Func(void)
{
	u32 tSignals;


	/* wait for events indefinitely and clear after end of function */
	tSignals = cfThreadWaitWithTimeout((EVENT_GLOBAL_Watchdog |
										EVENT_Key_TIMER_5MS_EVENT |
										EVENT_Key_TIMER_10MS_EVENT ),
										OS_WAITFOREVER);


	if(tSignals & EVENT_GLOBAL_Watchdog)
	{
		;
	}

}


TASK_INIT(Key)
{
	//SetAlarm(ALARM_NAME(PwrManage), 5, 10);


	enableCycleEvent((TASK_NAME(Key)));

}

TASK_CREATE(Key)
{
    cfThreadCreate(TASK_NAME(Key), KeyManage_Func, KeyManage_MsgHandle);
}




