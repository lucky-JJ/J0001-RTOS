#ifndef __OS_H__
#define __OS_H__

//#include "core_cm4.h"
#include "FreeRTOS.h"
#include "sys.h"
//#include "FwLib.h"

//#if (__CORTEX_M == 4)
//#include "Os_Cfg.h"
//#include "Alarm.h"

#include "task.h"
#include "queue.h"
#include "semphr.h"

//#include "Irq.h"
/*! @brief Type for a mutex. */
typedef SemaphoreHandle_t mutex_t;
/*! @brief Type for a semaphore. */
typedef SemaphoreHandle_t semaphore_t;

#define FREERTOS_MAX_TASK_NUM (9)

#define USE_TASK_MAIL_BOX_LIST (1)

#define OSIF_WAIT_FOREVER 0xFFFFFFFFu

#define OS_WAITFOREVER 0x0000

#define OS_DONOTWAIT 0xFFFF
//#endif

typedef struct msgpkt
{
#if (USE_TASK_MAIL_BOX_LIST == 1)
    ListItem_t Item;
#endif
    u16 datalen;
    u8 reserved[2];
} SMP_MSG;

typedef u16 MsgSize_t;
typedef struct EventTag
{
    volatile MsgSize_t Type;
    u8 _Reserved[2];
} Message_t;

/* Macro to declare ISR: always valid */
#define DeclareIsr(f)   \
    FUNC(void, OS_CODE) \
    f(void)
/* Macro to define ISR: always valid */
#define ISR(f)          \
    FUNC(void, OS_CODE) \
    f(void)

//#if (__CORTEX_M == 4)

//#define TRACE_ISR_ENTER() traceISR_ENTER()
//#define TRACE_ISR_EXIT() traceISR_EXIT()

typedef enum
{
    E_OS_OK,                   /* ((StatusType)0)  */
    E_OS_ACCESS,               /* ((StatusType)1)  */
    E_OS_CALLEVEL,             /* ((StatusType)2)  */
    E_OS_ID,                   /* ((StatusType)3)  */
    E_OS_LIMIT,                /* ((StatusType)4)  */
    E_OS_NOFUNC,               /* ((StatusType)5)  */
    E_OS_RESOURCE,             /* ((StatusType)6)  */
    E_OS_STATE,                /* ((StatusType)7)  */
    E_OS_VALUE,                /* ((StatusType)8)  */
    E_OS_SERVICEID,            /* ((StatusType)9)  */
    E_OS_ILLEGAL_ADDRESS,      /* ((StatusType)10) */
    E_OS_MISSINGEND,           /* ((StatusType)11) */
    E_OS_DISABLEDINT,          /* ((StatusType)12) */
    E_OS_STACKFAULT,           /* ((StatusType)13) */
    E_OS_PARAM_POINTER,        /* ((StatusType)14) */
    E_OS_PROTECTION_MEMORY,    /* ((StatusType)15) */
    E_OS_PROTECTION_TIME,      /* ((StatusType)16) */
    E_OS_PROTECTION_ARRIVAL,   /* ((StatusType)17) */
    E_OS_PROTECTION_LOCKED,    /* ((StatusType)18) */
    E_OS_PROTECTION_EXCEPTION, /* ((StatusType)19) */
    /* Spinlocks Errors */
    E_OS_SPINLOCK,              /* ((StatusType)20) */
    E_OS_INTERFERENCE_DEADLOCK, /* ((StatusType)21) */
    E_OS_NESTING_DEADLOCK,      /* ((StatusType)22) */
    /* Multicore */
    E_OS_CORE, /* ((StatusType)23) */
    /* Implementation specific errors: they must start with E_OS_SYS_ */
    /* Error during StartOS */
    E_OS_SYS_INIT,                  /* ((StatusType)24) */
    E_OS_SYS_SUSPEND_NESTING_LIMIT, /* (E_OS_SYS_INIT + 1U) */
    /* Error Codes for Dynamic APIs */
    E_OS_SYS_TASK,  /* (E_OS_SYS_INIT + 2U) */
    E_OS_SYS_STACK, /* (E_OS_SYS_INIT + 3U) */
    E_OS_SYS_ACT    /* (E_OS_SYS_INIT + 4U) */
} Os_status_type;

typedef Os_status_type Os_StatusType;

#define TASK(TaskName) void TASK_##TaskName(void *pvParameters)
#define TASK_NAME(name) (TID_##name)
#define TASK_MAIN(name) void TASK_MAIN_##name(void)
#define TASK_MSG_HANDLE(name) void TASK_MSG_HANDLE_##name(void)
#define TASK_CREATE(name) void TASK_CREATE_##name(void)

#define ALARM_NAME(name) (ALARM_ID_##name)
#define SEM_MTX_NAME(name) (SID_##name)

#define DisableOSInt()
#define EnableOSInt()

#define OsCounterMaxAllowedValue (0xffffffff)
/* See oil config for defines */
#define AppModeType s32

extern uint32_t *TaskList[];
#define EvtBits_t uint32_t

uint8_t EventSendTask(unsigned char Taskid, uint32_t Event);

uint8_t EventSendTaskFormISR(unsigned char Taskid, uint32_t Event);

void OS_HOOK_ErrorHook(uint8_t State, uint8_t ServiceId, uint8_t ID);

uint32_t EvtGroupsNotifyTask(const unsigned char TaskId, const uint32_t NotifyBits);

uint32_t EvtGroupsNotifyTaskFromISR(const unsigned char TaskId, const uint32_t NotifyBits);

uint32_t EvtGroupsWaitBits(const unsigned char TaskId, const uint32_t BitsToWaitFor);

void Os_TimeDelay(uint32_t delay);

u32 Os_MtxLock(u8 MutexId, const u32 timeout);

u32 Os_MtxUnlock(u8 MutexId);

u32 Os_SemWait(const u8 SemId, const u32 timeout);

u32 Os_SemPost(const u8 SemId);

void Os_IsrInit(void);

void InitOS(void);

void StartOS(void);

void ShutdownOS(u8 Error);

uint32_t Os_GetMilliseconds(void);

void Os_StartTick(void);

void Os_StopTick(void);

void Os_TaskResumeAll(void);

void Os_TaskSuspendAll(void);

void ResumeAllInterrupts(void);

void SuspendAllInterrupts(void);

const char *Os_GetCurTaskName(void);

u32 OS_ReceiveMailbox(u8 mbid, u8 **pBuf, u16 *pBufLen, u32 timeout);
u32 OS_SendMailbox(u8 mbid, u8 *pcMsg, u16 u16MsgLen);
u32 OS_MQSend(u8 tskId, u8 *pcMsg, u16 u16MsgLen);

//#endif
