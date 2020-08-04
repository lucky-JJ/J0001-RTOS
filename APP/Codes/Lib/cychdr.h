/*
 * @Author: your name
 * @Date: 2020-05-29 09:49:07
 * @LastEditTime: 2020-08-04 10:17:31
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \J0001-RTOS\APP\Codes\Lib\cychdr.h
 */

#include "sys.h"
#include "ResourceConfig.h"

#define CYCHDR_TICK 5

#define CYCLE_TIMER_EVNET_CFG                                    \
    {false, 1, 5, TASK_NAME(Uart1), EVENT_GLOBAL_TIMER_5MS},     \
        {false, 2, 10, TASK_NAME(Key), EVENT_GLOBAL_TIMER_10MS}, \
        {false, 3, 5, TASK_NAME(Lcd), EVENT_GLOBAL_TIMER_5MS},

extern void enableCycleEvent(u8 tskId);

extern void disableCycleEvent(u8 tskId);

extern void cycleTaskTick(void);

extern void cycleTaskInit(void);
