/******************************************************************************
 * Project          ChangAn S111
 * (c) copyright    2017
 * Company          chinatsp
 *                  All rights reserved
 *  Date            05.04.2017
 ******************************************************************************/
/**
 *  @file           taskhandl.h
 *  @ingroup        framework
 *  @author         huangtao
 *
 ******************************************************************************/
#ifndef _OSAL_H_
#define _OSAL_H_


/* --- include files ------------------------------------------------------ */
//#include "OSIF.h"
#include "sys.h"



#define TASK_NAME(name)                     (TID_##name)

#define MAIN_LOOP     for (;;) {
#define MAIN_LOOP_END }


typedef void (*TCfTaskFunc)(void);
typedef void (*TCfHandleMsgFunc)(void* pParam, u16 Len);

/*---------------------------------------------------------------------------
 * VARIABLES
 *---------------------------------------------------------------------------*/


#define InitOS    cfThreadInit
#define Os_IsrInit()

extern u32 cfThreadInit(void);

extern void cfThreadCreate(u8 tskId, TCfTaskFunc func, TCfHandleMsgFunc msgFunc);

extern u32  cfThreadWaitWithTimeout(u32 signals, u16 timeout);

#endif

