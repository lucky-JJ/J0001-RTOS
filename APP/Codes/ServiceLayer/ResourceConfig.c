#include "Os.h"
//#include "Alarm.h"
#include "ResourceConfig.h"
#include <stdio.h>
#include "queue.h"

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
extern void TASK_CanTask(void *pvParameters);
extern void TASK_BswTask(void *pvParameters);
extern void TASK_MiscTask(void *pvParameters);
extern void TASK_AudioTask(void *pvParameters);
extern void TASK_ComTask(void *pvParameters);
extern void TASK_SecTask(void *pvParameters);

/* module(swc) declaration function */
extern void SWC_PwrMgr_Create(void);
extern void SWC_PwrMgr_Init(void);
extern void SWC_PwrMgr_Main(void);
extern uint8_t SWC_PwrMgr_handle(uint8_t flag, uint32_t id);

extern void SWC_Time_Create(void);
extern void SWC_Time_Init(void);
extern void SWC_Time_Main(void);
extern uint8_t SWC_Time_handle(uint8_t flag, uint32_t id);

/* Module */

// SWC CanTask
static SwcDynData Swc_AirCtl_DynData =
    {
        0,
};
const Module_t Swc_AirCtl =
    {
        SWC_TRG_Periodic,    /* SWC Trigger Mode */
        20,                  /* Periodic */
        &Swc_AirCtl_DynData, /* SwcDynData */
        "AirCtl",            /* swc name */
        SWC_AirCtl_Create,
        SWC_AirCtl_Init,
        SWC_AirCtl_Main,
        SWC_AirCtl_handle,
};

const Module_t *SwcGrpOfCanTask[] =
    {
        &Swc_AirCtl,
        &Swc_Avm,
        &Swc_BCM,
        &Swc_Tbox,
        &Swc_CAN_PDU,
};

TaskDynData Dync_CanTask =
    {
        0,
        0,
};
const ModuleOfTask_t SwcsOfCanTask =
    {
        &Dync_CanTask,
        TASK_NAME(CanTask),
        5,
        (Module_t **)SwcGrpOfCanTask,
};

// SWC BswTask
static SwcDynData Swc_PwrMgr_DynData =
    {
        0,
};
const Module_t Swc_PwrMgr =
    {
        SWC_TRG_Periodic,    /* SWC Trigger Mode */
        10,                  /* Periodic */
        &Swc_PwrMgr_DynData, /* SwcDynData */
        "PwrMgr",            /* swc name */
        SWC_PwrMgr_Create,
        SWC_PwrMgr_Init,
        SWC_PwrMgr_Main,
        SWC_PwrMgr_handle,
};

static SwcDynData Swc_BswSrv_DynData =
    {
        0,
};
const Module_t Swc_BswSrv =
    {
        SWC_TRG_Periodic,    /* SWC Trigger Mode */
        10,                  /* Periodic */
        &Swc_BswSrv_DynData, /* SwcDynData */
        "BswSrv",            /* swc name */
        SWC_BswSrv_Create,
        SWC_BswSrv_Init,
        SWC_BswSrv_Main,
        SWC_BswSrv_handle,
};

const Module_t *SwcGrpOfBswTask[] =
    {
        &Swc_PwrMgr,
        &Swc_BswSrv,
};

TaskDynData Dync_BswTask =
    {
        0,
        0,
};
const ModuleOfTask_t SwcsOfBswTask =
    {
        &Dync_BswTask,
        TASK_NAME(BswTask),
        2,
        (Module_t **)SwcGrpOfBswTask,
};

// SWC MiscTask
static SwcDynData Swc_Time_DynData =
    {
        0,
};
const Module_t Swc_Time =
    {
        SWC_TRG_Periodic,  /* SWC Trigger Mode */
        100,               /* Periodic */
        &Swc_Time_DynData, /* SwcDynData */
        "Time",            /* swc name */
        SWC_Time_Create,
        SWC_Time_Init,
        SWC_Time_Main,
        SWC_Time_handle,
};

static SwcDynData Swc_KeyApp_DynData =
    {
        0,
};
const Module_t Swc_KeyApp =
    {
        SWC_TRG_Periodic,    /* SWC Trigger Mode */
        10,                  /* Periodic */
        &Swc_KeyApp_DynData, /* SwcDynData */
        "KeyApp",            /* swc name */
        SWC_KeyApp_Create,
        SWC_KeyApp_Init,
        SWC_KeyApp_Main,
        SWC_KeyApp_handle,
};

/* Task stk&tcb Buffer */

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
uint32_t TasksStaticCreate(void)
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
        TaskList[TID_UartTask] = (uint32_t *)xHandle;
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
        TaskList[TID_KeyTask] = (uint32_t *)xHandle;
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
        TaskList[TID_LedTask] = (uint32_t *)xHandle;
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
        TaskList[TID_LcdTask] = (uint32_t *)xHandle;
    }
    else
    {
        return 1;
    }

    xHandle = xTaskCreateStatic(TASK_ComTask,
                                "ComTask",
                                200,
                                NULL,
                                (configMAX_PRIORITIES - 1),
                                xTaskComTask_StkBuffer,
                                &xTaskComTask_TcbBuffer);
    if (xHandle != NULL)
    {
        TaskList[TID_ComTask] = (uint32_t *)xHandle;
    }
    else
    {
        return 1;
    }

    xHandle = xTaskCreateStatic(TASK_SecTask,
                                "SecTask",
                                200,
                                NULL,
                                (configMAX_PRIORITIES - 1),
                                xTaskSecTask_StkBuffer,
                                &xTaskSecTask_TcbBuffer);
    if (xHandle != NULL)
    {
        TaskList[TID_SecTask] = (uint32_t *)xHandle;
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
static List_t TaskMailBoxTbl[DID_Max];

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

    for (Index = 0; Index < DID_Max; Index++)
    {
        vListInitialise(&TaskMailBoxTbl[Index]);
    }

    return 0;
}

/**
 * @description: 组册任务
 * @param {type} 
 * @return: 
 */
void OS_ThreadCreate(void)
{
    TASK_CREATE_Uart1();
    TASK_CREATE_Key();
    TASK_CREATE_Led();
    TASK_CREATE_Lcd();

    /*
    SwcGroupHandleCreateFunction(&SwcsOfCanTask);
    SwcGroupHandleCreateFunction(&SwcsOfBswTask);
    SwcGroupHandleCreateFunction(&SwcsOfMiscTask);
    SwcGroupHandleCreateFunction(&SwcsOfAudioTask);
    SwcGroupHandleCreateFunction(&SwcsOfComTask);
    SwcGroupHandleCreateFunction(&SwcsOfSecTask);*/
}

typedef void (*TCfTaskFunc)(void);
typedef void (*TCfHandleMsgFunc)(void *pParam, u16 Len);
void cfThreadCreate(u8 tskId, TCfTaskFunc func, TCfHandleMsgFunc msgFunc)
{
    pFunc##osName = func;
    pMsgFunc##osName = msgFunc;
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

TASK(LedTask)
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

TASK(LcdTask)
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
uint32_t SemphrCreateStatic(void)
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