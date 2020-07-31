#include "Os.h"
//#include "Alarm.h"
#include "ResourceConfig.h"
#include <stdio.h>
#include "queue.h"
#include "list.h"

/* Message id bit table of every task   */
uint32_t TaskMsgReadyList[TASK_NUM][MESSAGE_LIST_SIZE(COM_RECEIVE_MESSAGE_COUNT, 32)];
uint32_t TaskMsgCnt[TASK_NUM][2];
uint32_t *TaskList[TASK_NUM];
//#if (MAX_SEM_AND_MTX_NUM > 0)
SemaphoreHandle_t SemphrTbl[MAX_SEM_AND_MTX_NUM];
//#endif

static StaticSemaphore_t xSemaphore_RtcMutex_buf;
static StaticSemaphore_t xSemaphore_I2C0Mutex_buf;
static StaticSemaphore_t xSemaphore_DbgMutex_buf;
static StaticSemaphore_t xSemaphore_I2C0Sem_buf;

/* os task declaration function */
extern void TASK_Uart1(void *pvParameters);
extern void TASK_Key(void *pvParameters);
extern void TASK_Led(void *pvParameters);
extern void TASK_Lcd(void *pvParameters);

/* Task stk&tcb Buffer */

//任务句柄
static TaskHandle_t TaskTcbTbl[TID_Max];

//任务堆栈大小
#define xTaskUartTask_STK_SIZE 200
#define xTaskKeyTask_STK_SIZE 50
#define xTaskLedTask_STK_SIZE 50
#define xTaskLcdTask_STK_SIZE 128

//任务堆栈 Stack
static StackType_t xTaskUartTask_StkBuffer[xTaskUartTask_STK_SIZE];
static StackType_t xTaskKeyTask_StkBuffer[xTaskKeyTask_STK_SIZE];
static StackType_t xTaskLedTask_StkBuffer[xTaskLedTask_STK_SIZE];
static StackType_t xTaskLcdTask_StkBuffer[xTaskLcdTask_STK_SIZE];

//任务控制块 TCB
static StaticTask_t xTaskUartTask_TcbBuffer;
static StaticTask_t xTaskKeyTask_TcbBuffer;
static StaticTask_t xTaskLedTask_TcbBuffer;
static StaticTask_t xTaskLcdTask_TcbBuffer;

/**
 * @description: 静态创建任务
 * @param {type} 
 * @return: 
 */
u8 TasksStaticCreate(void)
{
    TaskHandle_t xHandle = NULL;

    xHandle = xTaskCreateStatic(TASK_Uart1,                 //任务函数
                                "UartTask",                 //任务名称
                                xTaskUartTask_STK_SIZE,     //任务堆栈大小
                                NULL,                       //传递给任务函数的参数
                                (configMAX_PRIORITIES - 1), //任务优先级
                                xTaskUartTask_StkBuffer,    //任务堆栈
                                &xTaskUartTask_TcbBuffer);  //任务控制块
    if (xHandle != NULL)
    {
        TaskTcbTbl[TID_Uart1] = (uint32_t *)xHandle;
    }
    else
    {
        return 1;
    }

    xHandle = xTaskCreateStatic(TASK_Key,
                                "KeyTask",
                                xTaskKeyTask_STK_SIZE,
                                NULL,
                                (configMAX_PRIORITIES - 1),
                                xTaskKeyTask_StkBuffer,
                                &xTaskKeyTask_TcbBuffer);
    if (xHandle != NULL)
    {
        TaskTcbTbl[TID_Key] = (uint32_t *)xHandle;
    }
    else
    {
        return 1;
    }

    xHandle = xTaskCreateStatic(TASK_LedTask,
                                "LedTask",
                                xTaskLedTask_STK_SIZE,
                                NULL,
                                (configMAX_PRIORITIES - 1),
                                xTaskLedTask_StkBuffer,
                                &xTaskLedTask_TcbBuffer);
    if (xHandle != NULL)
    {
        TaskTcbTbl[TID_Led] = (uint32_t *)xHandle;
    }
    else
    {
        return 1;
    }

    xHandle = xTaskCreateStatic(TASK_LcdTask,
                                "LcdTask",
                                xTaskLcdTask_STK_SIZE,
                                NULL,
                                (configMAX_PRIORITIES - 1),
                                xTaskLcdTask_StkBuffer,
                                &xTaskLcdTask_TcbBuffer);
    if (xHandle != NULL)
    {
        TaskTcbTbl[TID_Lcd] = (uint32_t *)xHandle;
    }
    else
    {
        return 1;
    }

    return 0;
}

#define xUartQueueLength 256
#define xKeyQueueLength 32
#define xLedQueueLength 32
#define xLcdQueueLength 128

static u32 xQueueUartStroage[xUartQueueLength];
static u32 xQueueKeyStroage[xKeyQueueLength];
static u32 xQueueLedStroage[xLedQueueLength];
static u32 xQueueLcdStroage[xLcdQueueLength];

static StaticQueue_t xQueueBuf[DID_Max];
static QueueHandle_t TaskDataQueueTbl[configQUEUE_REGISTRY_SIZE];

#ifdef USE_TASK_MAIL_BOX_LIST
List_t TaskMailBoxTbl[DID_Max];
#endif

/**
 * @description: 静态创建数据队列
 * @param {type} 
 * @return: 
 */
u8 DataQueueCreateStatic(void)
{
    QueueHandle_t xQueue = NULL;
    u8 Index;

    xQueue = xQueueCreateStatic(xUartQueueLength, sizeof(u32), (u8 *)&xQueueUartStroage[0], &xQueueBuf[DID_Uart]);
    if (xQueue != NULL)
    {
#if (configQUEUE_REGISTRY_SIZE > 0)
        /* We want this queue to be viewable in a RTOS kernel aware debugger, so register it. */
        vQueueAddToRegistry(xQueue, "Uart");
#endif
        TaskDataQueueTbl[DID_Uart] = xQueue;
    }
    else
    {
        return 1;
    }

    xQueue = xQueueCreateStatic(xKeyQueueLength, sizeof(u32), (u8 *)&xQueueKeyStroage[0], &xQueueBuf[DID_Key]);
    if (xQueue != NULL)
    {
#if (configQUEUE_REGISTRY_SIZE > 0)
        /* We want this queue to be viewable in a RTOS kernel aware debugger, so register it. */
        vQueueAddToRegistry(xQueue, "Key");
#endif
        TaskDataQueueTbl[DID_Key] = xQueue;
    }
    else
    {
        return 1;
    }

    xQueue = xQueueCreateStatic(xLedQueueLength, sizeof(u32), (u8 *)&xQueueLedStroage[0], &xQueueBuf[DID_Led]);
    if (xQueue != NULL)
    {
#if (configQUEUE_REGISTRY_SIZE > 0)
        /* We want this queue to be viewable in a RTOS kernel aware debugger, so register it. */
        vQueueAddToRegistry(xQueue, "Uart");
#endif
        TaskDataQueueTbl[DID_Uart] = xQueue;
    }
    else
    {
        return 1;
    }

    xQueue = xQueueCreateStatic(xLcdQueueLength, sizeof(u32), (u8 *)&xQueueLcdStroage[0], &xQueueBuf[DID_Lcd]);
    if (xQueue != NULL)
    {
#if (configQUEUE_REGISTRY_SIZE > 0)
        /* We want this queue to be viewable in a RTOS kernel aware debugger, so register it. */
        vQueueAddToRegistry(xQueue, "Lcd");
#endif
        TaskDataQueueTbl[DID_Lcd] = xQueue;
    }
    else
    {
        return 1;
    }

#ifdef USE_TASK_MAIL_BOX_LIST
    for (Index = 0; Index < DID_Max; Index++)
    {
        vListInitialise(&TaskMailBoxTbl[Index]);
    }
#endif

    return 0;
}

/**
 * @description: 主线任务入口
 * @param {type} 
 * @return: 
 */
TASK(Key)
{
    EvtBits_t EvtBit;

    SwcGroupHandleInitFunction(&SwcsOfBswTask);
    MessageReadyListInit(&TaskMsgReadyList[TASK_NAME(KeyTask)][0], MESSAGE_LIST_SIZE(COM_RECEIVE_MESSAGE_COUNT, 32));
    for (;;)
    {
        EvtBit = EvtGroupsWaitBits(TASK_NAME(KeyTask),
                                   (EVT_ID_BswTask_COM_UPDATE | EVT_ID_BswTask_TIMER_5MS));

        if (EvtBit & EVT_ID_BswTask_COM_UPDATE)
        {
            SwcGroupHandleMessage(&SwcsOfBswTask, TASK_NAME(KeyTask));
        }
        else if (EvtBit & EVT_ID_BswTask_TIMER_5MS)
        {
            SwcGroupHandleMainFunction(&SwcsOfBswTask, 5);
        }
        else
        {
            SwcGroupHandleEvent(&SwcsOfBswTask, EvtBit);
        }
    }
}

TASK(Led)
{
    EvtBits_t EvtBit;

    SwcGroupHandleInitFunction(&SwcsOfMiscTask);
    MessageReadyListInit(&TaskMsgReadyList[TASK_NAME(LedTask)][0], MESSAGE_LIST_SIZE(COM_RECEIVE_MESSAGE_COUNT, 32));
    for (;;)
    {
        EvtBit = EvtGroupsWaitBits(TASK_NAME(LedTask),
                                   (EVT_ID_MiscTask_COM_UPDATE | EVT_ID_MiscTask_TIMER_10MS));

        if (EvtBit & EVT_ID_MiscTask_COM_UPDATE)
        {
            SwcGroupHandleMessage(&SwcsOfMiscTask, TASK_NAME(LedTask));
        }
        else if (EvtBit & EVT_ID_MiscTask_TIMER_10MS)
        {
            SwcGroupHandleMainFunction(&SwcsOfMiscTask, 10);
        }
        else
        {
            SwcGroupHandleEvent(&SwcsOfMiscTask, EvtBit);
        }
    }
}

TASK(Lcd)
{
    EvtBits_t EvtBit;

    SwcGroupHandleInitFunction(&SwcsOfAudioTask);
    MessageReadyListInit(&TaskMsgReadyList[TASK_NAME(LcdTask)][0], MESSAGE_LIST_SIZE(COM_RECEIVE_MESSAGE_COUNT, 32));
    for (;;)
    {
        EvtBit = EvtGroupsWaitBits(TASK_NAME(LcdTask),
                                   (EVT_ID_AudioTask_COM_UPDATE | EVT_ID_AudioTask_TIMER_10MS));

        if (EvtBit & EVT_ID_AudioTask_COM_UPDATE)
        {
            SwcGroupHandleMessage(&SwcsOfAudioTask, TASK_NAME(LcdTask));
        }
        else if (EvtBit & EVT_ID_AudioTask_TIMER_10MS)
        {
            SwcGroupHandleMainFunction(&SwcsOfAudioTask, 10);
        }
        else
        {
            SwcGroupHandleEvent(&SwcsOfAudioTask, EvtBit);
        }
    }
}

/**
 * @description: 创建静态互斥信号量 / 创建静态计数信号量
 * @param {type} 
 * @return: 返回一个句柄，通过该句柄可以引用新的信号量
 */
u8 SemphrCreateStatic(void)
{
    //#if (MAX_SEM_AND_MTX_NUM > 0)
    SemaphoreHandle_t xSemaphore = NULL;

    xSemaphore = xSemaphoreCreateMutexStatic(&xSemaphore_RtcMutex_buf);
    if (xSemaphore != NULL)
    {
        SemphrTbl[SID_RtcMutex] = xSemaphore;
    }
    else
    {
        return 1;
    }
    xSemaphore = xSemaphoreCreateMutexStatic(&xSemaphore_I2C0Mutex_buf);
    if (xSemaphore != NULL)
    {
        SemphrTbl[SID_I2C0Mutex] = xSemaphore;
    }
    else
    {
        return 1;
    }
    xSemaphore = xSemaphoreCreateMutexStatic(&xSemaphore_DbgMutex_buf);
    if (xSemaphore != NULL)
    {
        SemphrTbl[SID_DbgMutex] = xSemaphore;
    }
    else
    {
        return 1;
    }
    xSemaphore = xSemaphoreCreateCountingStatic(1, 0, &xSemaphore_I2C0Sem_buf);
    if (xSemaphore != NULL)
    {
        SemphrTbl[SID_I2C0Sem] = xSemaphore;
    }
    else
    {
        return 1;
    }

    //#endif
    return 0;
}