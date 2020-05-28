/******************************************************************************
 * Project          ChangAn S111
 * (c) copyright    2017
 * Company          chinatsp
 *                  All rights reserved
 *  Date            05.04.2017
 ******************************************************************************/
/**
 *  @file           Cychdr.c
 *  @ingroup        BIOS
 *  @author         huangtao
 *
 ******************************************************************************/


#include <kernel.h>
#include "kernel_id.h"
#include "osal.h"
#include "eventDefine.h"
#include "base_types.h"
#include "cychdr.h"
#include "bsp.h"

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

