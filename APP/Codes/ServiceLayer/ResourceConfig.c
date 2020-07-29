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

extern void SWC_AirCtl_Create(void);
extern void SWC_AirCtl_Init(void);
extern void SWC_AirCtl_Main(void);
extern uint8_t SWC_AirCtl_handle(uint8_t flag, uint32_t id);

extern void SWC_Avm_Create(void);
extern void SWC_Avm_Init(void);
extern void SWC_Avm_Main(void);
extern uint8_t SWC_Avm_handle(uint8_t flag, uint32_t id);

extern void SWC_BCM_Create(void);
extern void SWC_BCM_Init(void);
extern void SWC_BCM_Main(void);
extern uint8_t SWC_BCM_handle(uint8_t flag, uint32_t id);

extern void SWC_Tbox_Create(void);
extern void SWC_Tbox_Init(void);
extern void SWC_Tbox_Main(void);
extern uint8_t SWC_Tbox_handle(uint8_t flag, uint32_t id);

extern void SWC_BswSrv_Create(void);
extern void SWC_BswSrv_Init(void);
extern void SWC_BswSrv_Main(void);
extern uint8_t SWC_BswSrv_handle(uint8_t flag, uint32_t id);

extern void SWC_Security_Create(void);
extern void SWC_Security_Init(void);
extern void SWC_Security_Main(void);
extern uint8_t SWC_Security_handle(uint8_t flag, uint32_t id);

extern void SWC_CAN_PDU_Create(void);
extern void SWC_CAN_PDU_Init(void);
extern void SWC_CAN_PDU_Main(void);
extern uint8_t SWC_CAN_PDU_handle(uint8_t flag, uint32_t id);

extern void SWC_SOC_PDU_Create(void);
extern void SWC_SOC_PDU_Init(void);
extern void SWC_SOC_PDU_Main(void);
extern uint8_t SWC_SOC_PDU_handle(uint8_t flag, uint32_t id);

extern void SWC_KeyApp_Create(void);
extern void SWC_KeyApp_Init(void);
extern void SWC_KeyApp_Main(void);
extern uint8_t SWC_KeyApp_handle(uint8_t flag, uint32_t id);

extern void SWC_Audio_Create(void);
extern void SWC_Audio_Init(void);
extern void SWC_Audio_Main(void);
extern uint8_t SWC_Audio_handle(uint8_t flag, uint32_t id);

/* event list(swc) declaration function */
SwcEventMask_t TaskSwcEventList[TASK_NUM][MAX_EVENT_NUM] =
    {
        /* task:CanTask */
        {
            {0, {
                    0,
                }},
            {0, {
                    0,
                }},
            {0, {
                    0,
                }},
        },
        /* task:BswTask */
        {
            {0, {
                    0,
                }},
            {0, {
                    0,
                }},
            {0, {
                    0,
                }},
        },
        /* task:MiscTask */
        {
            {0, {
                    0,
                }},
            {0, {
                    0,
                }},
            {0, {
                    0,
                }},
        },
        /* task:AudioTask */
        {
            {0, {
                    0,
                }},
            {0, {
                    0,
                }},
            {0, {
                    0,
                }},
        },
        /* task:ComTask */
        {
            {0, {
                    0,
                }},
            {0, {
                    0,
                }},
            {1, {
                    SWC_NAME(SOC_PDU),
                }},
        },
        /* task:SecTask */
        {
            {0, {
                    0,
                }},
            {0, {
                    0,
                }},
            {0, {
                    0,
                }},
        },

};

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

static SwcDynData Swc_Avm_DynData =
    {
        0,
};
const Module_t Swc_Avm =
    {
        SWC_TRG_Periodic, /* SWC Trigger Mode */
        10,               /* Periodic */
        &Swc_Avm_DynData, /* SwcDynData */
        "Avm",            /* swc name */
        SWC_Avm_Create,
        SWC_Avm_Init,
        SWC_Avm_Main,
        SWC_Avm_handle,
};

static SwcDynData Swc_BCM_DynData =
    {
        0,
};
const Module_t Swc_BCM =
    {
        SWC_TRG_Periodic, /* SWC Trigger Mode */
        10,               /* Periodic */
        &Swc_BCM_DynData, /* SwcDynData */
        "BCM",            /* swc name */
        SWC_BCM_Create,
        SWC_BCM_Init,
        SWC_BCM_Main,
        SWC_BCM_handle,
};

static SwcDynData Swc_Tbox_DynData =
    {
        0,
};
const Module_t Swc_Tbox =
    {
        SWC_TRG_Periodic,  /* SWC Trigger Mode */
        10,                /* Periodic */
        &Swc_Tbox_DynData, /* SwcDynData */
        "Tbox",            /* swc name */
        SWC_Tbox_Create,
        SWC_Tbox_Init,
        SWC_Tbox_Main,
        SWC_Tbox_handle,
};

static SwcDynData Swc_CAN_PDU_DynData =
    {
        0,
};
const Module_t Swc_CAN_PDU =
    {
        SWC_TRG_Periodic,     /* SWC Trigger Mode */
        5,                    /* Periodic */
        &Swc_CAN_PDU_DynData, /* SwcDynData */
        "CAN_PDU",            /* swc name */
        SWC_CAN_PDU_Create,
        SWC_CAN_PDU_Init,
        SWC_CAN_PDU_Main,
        SWC_CAN_PDU_handle,
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

const Module_t *SwcGrpOfMiscTask[] =
    {
        &Swc_Time,
        &Swc_KeyApp,
};

TaskDynData Dync_MiscTask =
    {
        0,
        0,
};
const ModuleOfTask_t SwcsOfMiscTask =
    {
        &Dync_MiscTask,
        TASK_NAME(MiscTask),
        2,
        (Module_t **)SwcGrpOfMiscTask,
};

// SWC AudioTask
static SwcDynData Swc_Audio_DynData =
    {
        0,
};
const Module_t Swc_Audio =
    {
        SWC_TRG_Periodic,   /* SWC Trigger Mode */
        10,                 /* Periodic */
        &Swc_Audio_DynData, /* SwcDynData */
        "Audio",            /* swc name */
        SWC_Audio_Create,
        SWC_Audio_Init,
        SWC_Audio_Main,
        SWC_Audio_handle,
};

const Module_t *SwcGrpOfAudioTask[] =
    {
        &Swc_Audio,
};

TaskDynData Dync_AudioTask =
    {
        0,
        0,
};
const ModuleOfTask_t SwcsOfAudioTask =
    {
        &Dync_AudioTask,
        TASK_NAME(AudioTask),
        1,
        (Module_t **)SwcGrpOfAudioTask,
};

// SWC ComTask
static SwcDynData Swc_SOC_PDU_DynData =
    {
        0,
};
const Module_t Swc_SOC_PDU =
    {
        SWC_TRG_Periodic,     /* SWC Trigger Mode */
        10,                   /* Periodic */
        &Swc_SOC_PDU_DynData, /* SwcDynData */
        "SOC_PDU",            /* swc name */
        SWC_SOC_PDU_Create,
        SWC_SOC_PDU_Init,
        SWC_SOC_PDU_Main,
        SWC_SOC_PDU_handle,
};

const Module_t *SwcGrpOfComTask[] =
    {
        &Swc_SOC_PDU,
};

TaskDynData Dync_ComTask =
    {
        0,
        0,
};
const ModuleOfTask_t SwcsOfComTask =
    {
        &Dync_ComTask,
        TASK_NAME(ComTask),
        1,
        (Module_t **)SwcGrpOfComTask,
};

// SWC SecTask
static SwcDynData Swc_Security_DynData =
    {
        0,
};
const Module_t Swc_Security =
    {
        SWC_TRG_Periodic,      /* SWC Trigger Mode */
        5,                     /* Periodic */
        &Swc_Security_DynData, /* SwcDynData */
        "Security",            /* swc name */
        SWC_Security_Create,
        SWC_Security_Init,
        SWC_Security_Main,
        SWC_Security_handle,
};

const Module_t *SwcGrpOfSecTask[] =
    {
        &Swc_Security,
};

TaskDynData Dync_SecTask =
    {
        0,
        0,
};
const ModuleOfTask_t SwcsOfSecTask =
    {
        &Dync_SecTask,
        TASK_NAME(SecTask),
        1,
        (Module_t **)SwcGrpOfSecTask,
};

/* Task stk&tcb Buffer */
static StackType_t xTaskCanTask_StkBuffer[200];
static StaticTask_t xTaskCanTask_TcbBuffer;
static StackType_t xTaskBswTask_StkBuffer[200];
static StaticTask_t xTaskBswTask_TcbBuffer;
static StackType_t xTaskMiscTask_StkBuffer[200];
static StaticTask_t xTaskMiscTask_TcbBuffer;
static StackType_t xTaskAudioTask_StkBuffer[200];
static StaticTask_t xTaskAudioTask_TcbBuffer;
static StackType_t xTaskComTask_StkBuffer[200];
static StaticTask_t xTaskComTask_TcbBuffer;
static StackType_t xTaskSecTask_StkBuffer[200];
static StaticTask_t xTaskSecTask_TcbBuffer;

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

    xHandle = xTaskCreateStatic(TASK_UartTask,              //任务函数
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

    xHandle = xTaskCreateStatic(TASK_KeyTask,
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

QueueHandle_t TaskDataQueueTbl[configQUEUE_REGISTRY_SIZE];
QueueHandle_t TaskMailBoxTbl[DID_Max];

#define xUartQueueLength 256
StaticQueue_t xQueueUartBuf;

StaticQueue_t xQueueBuf[DID_Max];

u8 DataQueueCreateStatic(void)
{
    QueueHandle_t xQueue = NULL;
    u8 Index;

    xQueue = xQueueCreateStatic(xUartQueueLength, sizeof(u32), (u8 *)&xQueueUartStroage[0], &xQueueUartBuf);
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

    for (Index = 0; Index < DID_Max; Index++)
    {

        //xQueue = xQueueCreateStatic();

        vListInitialise(&TaskMailBoxTbl[Index]);
    }

    return 0;
}

void SwcGroupInit(void)
{
    SwcGroupHandleCreateFunction(&SwcsOfCanTask);
    SwcGroupHandleCreateFunction(&SwcsOfBswTask);
    SwcGroupHandleCreateFunction(&SwcsOfMiscTask);
    SwcGroupHandleCreateFunction(&SwcsOfAudioTask);
    SwcGroupHandleCreateFunction(&SwcsOfComTask);
    SwcGroupHandleCreateFunction(&SwcsOfSecTask);
}

/* TASK Function */
TASK(UartTask)
{
    EvtBits_t EvtBit;

    SwcGroupHandleInitFunction(&SwcsOfCanTask);
    MessageReadyListInit(&TaskMsgReadyList[TASK_NAME(UartTask)][0], MESSAGE_LIST_SIZE(COM_RECEIVE_MESSAGE_COUNT, 32));
    for (;;)
    {
        EvtBit = EvtGroupsWaitBits(TASK_NAME(UartTask),
                                   (EVT_ID_CanTask_COM_UPDATE | EVT_ID_All_Task_TIMER_5MS));

        if (EvtBit & EVT_ID_CanTask_COM_UPDATE)
        {
            SwcGroupHandleMessage(&SwcsOfCanTask, TASK_NAME(UartTask));
        }
        else if (EvtBit & EVT_ID_All_Task_TIMER_5MS)
        {
            SwcGroupHandleMainFunction(&SwcsOfCanTask, 5);
        }
        else
        {
            SwcGroupHandleEvent(&SwcsOfCanTask, EvtBit);
        }
    }
}

TASK(KeyTask)
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