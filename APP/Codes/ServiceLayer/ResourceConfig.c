
//#include "Alarm.h"
#include "ResourceConfig.h"
#include <stdio.h>
#include "list.h"
#include "Irq.h"

/* os task declaration function */
extern void TASK_Uart1(void *pvParameters);
extern void TASK_Key(void *pvParameters);
extern void TASK_Led(void *pvParameters);
extern void TASK_Lcd(void *pvParameters);
/*======================================================================================*/
/*======================================================================================*/
/* Task stk&tcb Buffer */

//任务句柄
TaskHandle_t TaskTcbTbl[TID_Max];
uint32_t *TaskList[TID_Max];

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
        //TaskTcbTbl[TID_Uart1] = (uint32_t *)xHandle;
        TaskList[TID_Uart1] = (uint32_t *)xHandle;

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
        TaskList[TID_Key] = (uint32_t *)xHandle;
    }
    else
    {
        return 1;
    }

    xHandle = xTaskCreateStatic(TASK_Led,
                                "LedTask",
                                xTaskLedTask_STK_SIZE,
                                NULL,
                                (configMAX_PRIORITIES - 1),
                                xTaskLedTask_StkBuffer,
                                &xTaskLedTask_TcbBuffer);
    if (xHandle != NULL)
    {
        TaskList[TID_Led] = (uint32_t *)xHandle;
    }
    else
    {
        return 1;
    }

    xHandle = xTaskCreateStatic(TASK_Lcd,
                                "LcdTask",
                                xTaskLcdTask_STK_SIZE,
                                NULL,
                                (configMAX_PRIORITIES - 1),
                                xTaskLcdTask_StkBuffer,
                                &xTaskLcdTask_TcbBuffer);
    if (xHandle != NULL)
    {
        TaskList[TID_Lcd] = (uint32_t *)xHandle;
    }
    else
    {
        return 1;
    }

    return 0;
}
/*======================================================================================*/
/*======================================================================================*/
#define xUartQueueLength 256
#define xKeyQueueLength 32
#define xLedQueueLength 32
#define xLcdQueueLength 128

static u32 xQueueUartStroage[xUartQueueLength];
static u32 xQueueKeyStroage[xKeyQueueLength];
static u32 xQueueLedStroage[xLedQueueLength];
static u32 xQueueLcdStroage[xLcdQueueLength];

static StaticQueue_t xQueueBuf[DID_Max];

//队列句柄
#if (USE_TASK_MAIL_BOX_LIST == 1)
List_t TaskMailBoxList[DID_Max];
#else
QueueHandle_t TaskDataQueueTbl[configQUEUE_REGISTRY_SIZE];
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

#if (USE_TASK_MAIL_BOX_LIST == 0)
        TaskDataQueueTbl[DID_Uart] = xQueue;
#endif
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

#if (USE_TASK_MAIL_BOX_LIST == 0)
        TaskDataQueueTbl[DID_Key] = xQueue;
#endif
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
        vQueueAddToRegistry(xQueue, "Led");
#endif
        
#if (USE_TASK_MAIL_BOX_LIST == 0)
        TaskDataQueueTbl[DID_Led] = xQueue;
#endif
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
       
#if (USE_TASK_MAIL_BOX_LIST == 0)
        TaskDataQueueTbl[DID_Lcd] = xQueue;
#endif
    }
    else
    {
        return 1;
    }

#if (USE_TASK_MAIL_BOX_LIST == 1)
    for (Index = 0; Index < DID_Max; Index++)
    {
        vListInitialise(&TaskMailBoxList[Index]);
    }
#endif

    return 0;
}
/*======================================================================================*/
/*======================================================================================*/
SemaphoreHandle_t SemphrTbl[SID_Max];

static StaticSemaphore_t xSemaphore_RtcMutex_buf;
static StaticSemaphore_t xSemaphore_I2CMutex_buf;
static StaticSemaphore_t xSemaphore_DbgMutex_buf;
static StaticSemaphore_t xSemaphore_I2C0Sem_buf;

/**
 * @description: 创建静态互斥信号量 / 创建静态计数信号量
 * @param {type} 
 * @return: 返回一个句柄，通过该句柄可以引用新的信号量
 */
u8 SemphrCreateStatic(void)
{
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
    xSemaphore = xSemaphoreCreateMutexStatic(&xSemaphore_I2CMutex_buf);
    if (xSemaphore != NULL)
    {
        SemphrTbl[SID_I2CMutex] = xSemaphore;
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

    return 0;
}

/**
 * Initialization of kernel structures and start of the first
 * task.
 */
void InitOS(void)
{
    __disable_irq();
    /* event group init */
    /* sem create init */
    SemphrCreateStatic();
    /* queue create init */
    DataQueueCreateStatic();
    /* mutex create init */
    TasksStaticCreate();
    /*alarm init*/
    //Alarm_Init();

    __enable_irq();
    // Now all tasks should be created.
}
