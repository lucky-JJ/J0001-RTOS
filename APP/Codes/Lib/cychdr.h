


#include "osal.h"
#include "eventDefine.h"
#include "sys.h"
#include "TaskId.h"



#define CYCHDR_TICK            5

#define CYCLE_TIMER_EVNET_CFG        \
	{false , 1 , 5  , TASK_NAME(Key), 	EVENT_Key_TIMER_5MS_EVENT   },\
	{false , 1 , 10 , TASK_NAME(Key), 	EVENT_Key_TIMER_10MS_EVENT  },\
	{false , 1 , 5  , TASK_NAME(Led), 	EVENT_Led_TIMER_5MS_EVENT   },\
	{false , 1 , 20 , TASK_NAME(Led), 	EVENT_Led_TIMER_20MS_EVENT  },\
	{false , 1 , 5  , TASK_NAME(Uart1),	EVENT_Uart1_TIMER_5MS_EVENT },


extern void enableCycleEvent(u8 tskId);

extern void disableCycleEvent(u8 tskId);

extern void cycleTaskTick(void);

extern void cycleTaskInit(void);




