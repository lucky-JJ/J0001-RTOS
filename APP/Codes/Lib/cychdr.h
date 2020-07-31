

//#include "osal.h"
#include "eventDefine.h"
#include "sys.h"
#include "TaskId.h"
#include "Os.h"

#define CYCHDR_TICK 5

#define CYCLE_TIMER_EVNET_CFG                                    \
    {false, 1, 5, TASK_NAME(Uart1), EVENT_GLOBAL_TIMER_5MS},     \
        {false, 2, 10, TASK_NAME(Key), EVENT_GLOBAL_TIMER_10MS}, \
        {false, 3, 5, TASK_NAME(Led), EVENT_GLOBAL_TIMER_5MS},   \
        {false, 4, 5, TASK_NAME(Lcd), EVENT_GLOBAL_TIMER_5MS},

extern void enableCycleEvent(u8 tskId);

extern void disableCycleEvent(u8 tskId);

extern void cycleTaskTick(void);

extern void cycleTaskInit(void);
