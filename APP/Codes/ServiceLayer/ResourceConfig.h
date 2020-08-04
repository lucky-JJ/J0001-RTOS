/*
 * @Author: J
 * @Date: 2020-07-28 10:59:07
 * @LastEditTime: 2020-08-04 10:12:12
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \APP\Codes\ServiceLayer\ResourceConfig.h
 */
#ifndef RESOURCE_CONFIG_H
#define RESOURCE_CONFIG_H

#include "OS.h"

enum
{
    /* os task id config */
    TID_Uart1 = 1,
    TID_Key,
    TID_Led,
    TID_Lcd,
    TID_Max,

    /* semaphore id config  */
    SID_RtcMutex = 0,
    SID_I2C0Mutex,
    SID_DbgMutex,
    SID_I2C0Sem,
    SID_Max,

    /*Data Queue ID*/
    DID_Uart = 1,
    DID_Key,
    DID_Led,
    DID_Lcd,
    DID_Max,

    /*MailBox ID*/

    /*Mutex ID*/
};

#define TASK(Name) void TASK_##Name(void *pvParameters)
#define TASK_NAME(name) (TID_##name)
#define TASK_MAIN(name) void TASK_MAIN_##name(void)
#define TASK_INIT(name) void TASK_INIT_##name(void)
#define TASK_MSG_HANDLE(name) void TASK_MSG_HANDLE_##name(void)
#define TASK_CREATE(name) void TASK_CREATE_##name(void)

#define ALARM_NAME(name) (ALARM_ID_##name)
#define SEM_MTX_NAME(name) (SID_##name)




/* event config */
#define EVENT_GLOBAL_MAILBOX (1 << 0)    // for All Task
#define EVENT_GLOBAL_Watchdog (1 << 1)   // for All Task
#define EVENT_GLOBAL_TIMER_5MS (1 << 2)  // for All Task
#define EVENT_GLOBAL_TIMER_10MS (1 << 3) // for All Task



extern SemaphoreHandle_t SemphrTbl[SID_Max];

//任务句柄
//extern TaskHandle_t TaskTcbTbl[TID_Max];
extern uint32_t *TaskList[TID_Max];


//邮箱句柄
#if (USE_TASK_MAIL_BOX_LIST == 1)
    extern List_t TaskMailBoxTbl[DID_Max];
#else
    extern QueueHandle_t TaskDataQueueTbl[configQUEUE_REGISTRY_SIZE];
#endif

void InitOS(void);
#endif
