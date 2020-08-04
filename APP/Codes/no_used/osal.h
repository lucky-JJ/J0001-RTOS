/*
 * @Author: your name
 * @Date: 2020-7-28 10:09:39
 * @LastEditTime: 2020-07-31 17:47:57
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \APP\Codes\ServiceLayer\osal.h
 */

#ifndef _OSAL_H_
#define _OSAL_H_

/* --- include files ------------------------------------------------------ */
//#include "OSIF.h"
#include "sys.h"

//#define TASK_NAME(name) (TID_##name)

#define MAIN_LOOP \
    for (;;)      \
    {
#define MAIN_LOOP_END }

typedef void (*TCfTaskFunc)(void);
typedef void (*TCfHandleMsgFunc)(void *pParam, u16 Len);

/*---------------------------------------------------------------------------
 * VARIABLES
 *---------------------------------------------------------------------------*/

//#define InitOS cfThreadInit
#define Os_IsrInit()

extern u32 cfThreadInit(void);

extern void cfThreadCreate(u8 tskId, TCfTaskFunc func, TCfHandleMsgFunc msgFunc);

extern u32 cfThreadWaitWithTimeout(u32 signals, u16 timeout);

#endif
