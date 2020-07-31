/*
 * @Author: J
 * @Date: 2020-07-28 10:59:07
 * @LastEditTime: 2020-07-31 18:07:23
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \APP\Codes\ServiceLayer\ResourceConfig.h
 */
#ifndef RESOURCE_CONFIG_H
#define RESOURCE_CONFIG_H

enum
{
    /* os task id config */
    TID_Uart1 = 1,
    TID_Key,
    TID_Led,
    TID_Lcd,
    TID_Max,

    /* semaphore id config  */

    /*Data Queue ID*/
    DID_Uart = 1,
    DID_Key,
    DID_Led,
    DID_Lcd,
    DID_Max,

    /*MailBox ID*/

    /*Mutex ID*/
};

/* semaphore id config  */
#define SID_RtcMutex (0)
#define SID_I2C0Mutex (1)
#define SID_DbgMutex (2)
#define SID_I2C0Sem (3)

#define MAX_SEM_AND_MTX_NUM (4)

/* event config */

#define EVENT_GLOBAL_MAILBOX (1 << 0)    // for All Task
#define EVENT_GLOBAL_Watchdog (1 << 1)   // for All Task
#define EVENT_GLOBAL_TIMER_5MS (1 << 2)  // for All Task
#define EVENT_GLOBAL_TIMER_10MS (1 << 3) // for All Task

#define TASK_NUM (6)
#define MAX_SWC_NUM (12)
#define MAX_EVENT_NUM (3)

#if (MAX_SEM_AND_MTX_NUM > 0)
extern SemaphoreHandle_t SemphrTbl[MAX_SEM_AND_MTX_NUM];
#endif

u8 TasksStaticCreate(void);
u8 SemphrCreateStatic(void);
u8 DataQueueCreateStatic(void);
#endif
