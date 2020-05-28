#ifndef OSIF_FREERTOS_H
#define OSIF_FREERTOS_H

/* FreeRTOS implementation */
#include "sys.h"
#include "TasKId.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

/*! @brief Type for a mutex. */
typedef SemaphoreHandle_t mutex_t;
/*! @brief Type for a semaphore. */
typedef SemaphoreHandle_t semaphore_t;

/*! @endcond */

#define FEATURE_OSIF_FREERTOS_ISR_CONTEXT_METHOD         (1) /* Cortex M device */




#define OSIF_WAIT_FOREVER 0xFFFFFFFFu


#define OS_WAITFOREVER     0x0000

#define OS_DONOTWAIT       0xFFFF

typedef  void (*pTsk_Handler)(u32 exinf);


typedef struct  msgpkt
{
    u16     datalen;
	u8      reserved[2];
} SMP_MSG;

#define OSIF_FreeMailBoxMemory(pBuf)  (vPortFree(pBuf - sizeof(SMP_MSG)))

#define OSIF_BASETASK(TaskName)  void TaskName##_Task(void *pvParameters)

#define DisableOSInt(Int)
#define EnableOSInt(Int)

u32 OSIF_Init(void);

void OSIF_TimeDelay(u32 delay);

u32 OSIF_GetMilliseconds(void);

u32 OSIF_MutexLock(u8 MutexId, const u32 timeout);

u32 OSIF_MutexUnlock(u8 MutexId);

u32 OSIF_SemaWait(const u8 SemId, const u32 timeout);

u32 OSIF_SemaPost(const u8 SemId);

u32 OSIF_ActivateTsk(u8 tskId);

u32 OSIF_TerminateTsk(u8 tskId);

u8 OSIF_GetActiveTask(void);

u32 OSIF_EVSendTask(u8 tskId, s32 event);

u32 OSIF_WaitEvent(u32 inEvents, u32 timeout);

u8  OSIF_GetMailboxPool(u8 tskId);

u32 OSIF_ReceiveMailbox(u8 mbid, u8 **pBuf, u16 *pBufLen, u32 timeout);

u32 OSIF_MQSend(u8 tskId, u8 *pcMsg, u16 u16MsgLen);

void OS_HOOK_ErrorHook(u8 State, u8 ServiceId, u8 ID);

#endif
