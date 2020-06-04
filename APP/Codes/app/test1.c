#include "osal.h"
#include "osif_freertos.h"
#include "app.h"
#include "sys.h"
#include "EventDefine.h"
#include "cychdr.h"




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
										EVENT_TIMER_5MS |
										EVENT_TIMER_10MS ),
										OS_WAITFOREVER);


	if(tSignals & EVENT_GLOBAL_Watchdog)
	{
		;
	}

}


TASK_INIT(Key)
{



	enableCycleEvent((TASK_NAME(Key)));

}

TASK_CREATE(Key)
{
    cfThreadCreate(TASK_NAME(Key), KeyManage_Func, KeyManage_MsgHandle);
}




