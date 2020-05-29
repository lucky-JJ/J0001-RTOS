
#include "osif_freertos.h"
#include "cychdr.h"


typedef struct
{
	bool   bEnable;
    u32    initTick;
	u16    periodTtick;
    u8     tskId;
	s32    event;

} CycleFrame;


static CycleFrame stCycleTask[] =
{
    CYCLE_TIMER_EVNET_CFG
};

void cycleTaskInit(void)
{
    CycleFrame *pCycleTask;
    u8 index;
    u8 tickNum = sizeof(stCycleTask) / sizeof(CycleFrame);


    for(index = 0; index < tickNum; index++)
    {
        pCycleTask = &stCycleTask[index];
        pCycleTask->bEnable = false;
    }
}

void cycleTaskTick(void)
{
    CycleFrame *pCycleTask;
    u8 index;
    u8 tickNum = sizeof(stCycleTask) / sizeof(CycleFrame);


    for(index = 0; index < tickNum; index++)
    {
        pCycleTask = &stCycleTask[index];

        if(pCycleTask->bEnable)
        {
            if(pCycleTask->initTick >= CYCHDR_TICK)
            {
                pCycleTask->initTick -= CYCHDR_TICK;
            }
            else
            {
                pCycleTask->initTick = 0;
            }

            if(0 == pCycleTask->initTick)
            {
                OSIF_EVSendTask(pCycleTask->tskId, pCycleTask->event);

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


    for(index = 0; index < tickNum; index++)
    {
        pCycleTask = &stCycleTask[index];

        if(tskId == pCycleTask->tskId)
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


    for(index = 0; index < tickNum; index++)
    {
        pCycleTask = &stCycleTask[index];

        if(tskId == pCycleTask->tskId)
        {
            pCycleTask->bEnable = false;
        }
    }
}

