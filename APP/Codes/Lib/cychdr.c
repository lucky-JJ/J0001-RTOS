/*
 * @Author: your name
 * @Date: 2020-05-29 09:49:07
 * @LastEditTime: 2020-08-05 15:31:13
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \J0001-RTOS\APP\Codes\Lib\cychdr.c
 */

#include "cychdr.h"

typedef struct
{
    bool bEnable;
    u32 initTick;
    u16 periodTtick;
    u8 tskId;
    s32 event;

} CycleFrame;

static CycleFrame stCycleTask[] = {CYCLE_TIMER_EVNET_CFG};

void cycleTaskInit(void)
{
    CycleFrame *pCycleTask;
    u8 index;
    u8 tickNum = sizeof(stCycleTask) / sizeof(CycleFrame);

    for (index = 0; index < tickNum; index++)
    {
        pCycleTask = &stCycleTask[index];
        pCycleTask->bEnable = false;
    }
}

/**
 * @description:在定时器中断中周期调用, 循环触发任务事件 
 * @param {type} 
 * @return: 
 */
void cycleTaskTick(void)
{
    CycleFrame *pCycleTask;
    u8 index;
    u8 tickNum = sizeof(stCycleTask) / sizeof(CycleFrame);

    for (index = 0; index < tickNum; index++)
    {
        pCycleTask = &stCycleTask[index];

        if (pCycleTask->bEnable)
        {
            if (pCycleTask->initTick >= CYCHDR_TICK)
            {
                pCycleTask->initTick -= CYCHDR_TICK;
            }
            else
            {
                pCycleTask->initTick = 0;
            }

            if (0 == pCycleTask->initTick)
            {
                Os_EventSendTask(pCycleTask->tskId, pCycleTask->event);

                pCycleTask->initTick = pCycleTask->periodTtick;
            }
        }
    }
}

void enableCycleEvent(u8 tskId)
{
    CycleFrame *pCycleTask;
    u8 index;
    u8 tickNum = sizeof(stCycleTask) / sizeof(CycleFrame);

    for (index = 0; index < tickNum; index++)
    {
        pCycleTask = &stCycleTask[index];

        if (tskId == pCycleTask->tskId)
        {
            pCycleTask->bEnable = true;
        }
    }
}

void disableCycleEvent(u8 tskId)
{
    CycleFrame *pCycleTask;
    u8 index;
    u8 tickNum = sizeof(stCycleTask) / sizeof(CycleFrame);

    for (index = 0; index < tickNum; index++)
    {
        pCycleTask = &stCycleTask[index];

        if (tskId == pCycleTask->tskId)
        {
            pCycleTask->bEnable = false;
        }
    }
}

/**
 * @description: OS 定时器回调函数,1ms一次
 * @param {type} 
 * @return: 
 */
void vApplicationTickHook(void)
{
    static u8 cnt = 0;

    if (++cnt == 5)
    {
        cnt = 0;
        cycleTaskTick(); /*5ms 进一次*/
    }
}
