/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * THIS SOFTWARE IS PROVIDED BY NXP "AS IS" AND ANY EXPRESSED OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL NXP OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS u32ERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

/*!
 * @file osif_freertos.c
 *
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 1.3, Taking address of near auto variable
 * The code is not dynamically linked. An absolute stack address is obtained when
 * taking the address of the near auto variable. A source of error in writing
 * dynamic code is that the stack segment may be different from the data segment.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Directive 4.9, Function-like macro defined.
 * The macros are used to validate input parameters to driver functions.
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 7.2, Unsigned u32eger literal without a 'U' suffix
 * Register address defined by FreeRTOS header files.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 8.7, External could be made static.
 * Function is defined for usage by application code.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 8.13, Pou32er variable could be declared as pou32ing to const
 * Type definition is done in FreeRTOS header files.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 11.4, Conversion between a pou32er and u32eger type.
 * The cast is required to initialize a pou32er with an unsigned long define, representing an address.
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 11.6, Cast from unsigned u32 to pou32er.
 * This is required for initializing pou32ers to the module's memory map, which is located at a
 * fixed address.
 *
 */

#include "osif_freertos.h"
#include "eventDefine.h"
#include <string.h>
#include <stdbool.h>

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Private Functions
 ******************************************************************************/
/* ----------------local function prototypes -------------------------------*/

/*! @cond DRIVER_u32ERNAL_USE_ONLY */

/*! @brief Converts milliseconds to ticks*/
#define MSEC_TO_TICK(msec) (pdMS_TO_TICKS(msec))

/*FUNCTION**********************************************************************
 *
 * Function Name : osif_IsIsrContext
 * Description   : This function returns true if the current execution context
 *  is from an ISR and false if execution is in normal thread mode.
 *
 *END**************************************************************************/
#if FEATURE_OSIF_FREERTOS_ISR_CONTEXT_METHOD == 1
/* Cortex M device - read ICSR[IPSR] value */
static inline bool osif_IsIsrContext(void)
{
    bool is_isr = false;
    u32 ipsr_code = (u32)((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) >> SCB_ICSR_VECTACTIVE_Pos);
    if (ipsr_code != 0u)
    {
        is_isr = true;
    }

    return is_isr;
}
#elif FEATURE_OSIF_FREERTOS_ISR_CONTEXT_METHOD == 2
/* PowerPC device, for FreeRTOS 9.0.0 read the SPRG0 reg that denotes the u32errupt nesting level */
#define SPRG0_ADDR (272)
static inline bool osif_IsIsrContext(void)
{
    u32 u32errupt_level = MFSPR(SPRG0_ADDR);
    bool is_isr = (bool)(u32errupt_level > 0u);
    return is_isr;
    return 1;
}
#else
#error "No method to check ISR Context"
#endif /* FEATURE_OSIF_FREERTOS_ISR_CONTEXT_METHOD */
/*! @endcond */

/*******************************************************************************
 * Code
 ******************************************************************************/
u32 OSIF_Init(void)
{
    u32 RetVal = 0;

    RetVal = TasksCreateStatic(); //创建任务
    if (RetVal == 0)
    {
        RetVal = DataQueueCreateStatic(); //创建队列
    }

    if (RetVal == 0)
    {
        RetVal = SemphrCreateStatic(); //创建信号量
    }

    return RetVal;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : OSIF_TimeDelay
 * Description   : This function blocks (sleep) the current thread for a number
 *  of milliseconds.
 *
 * Implements : OSIF_TimeDelay_freertos_Activity
 *END**************************************************************************/
void OSIF_TimeDelay(u32 delay)
{
    /* One dependency for FreeRTOS config file */
    /* INCLUDE_vTaskDelay */
    vTaskDelay(MSEC_TO_TICK(delay));
}

/*FUNCTION**********************************************************************
 *
 * Function Name : OSIF_GetMilliseconds
 * Description   : This function returns the number of miliseconds elapsed since
 *                  starting the u32ernal timer (since scheduler was started).
 *
 * Implements : OSIF_GetMilliseconds_freertos_Activity
 *END**************************************************************************/
u32 OSIF_GetMilliseconds(void)
{
    /*
     * Return the tick count in miliseconds
     * Note: if configTICK_RATE_HZ is less than 1000, the return value will be truncated
     * to 32-bit wide for large values of the tick count.
     */
    return (u32)((((u64)xTaskGetTickCount()) * 1000u) / configTICK_RATE_HZ);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : OSIF_MutexLock
 * Description   : This function obtains the mutex lock or returns error if timeout.
 *
 * Implements : OSIF_MutexLock_freertos_Activity
 *END**************************************************************************/
u32 OSIF_MutexLock(u8 MutexId, const u32 timeout)
{
    u32 timeoutTicks;
    u32 osif_ret_code = 1;
    TaskHandle_t mutex_holder_handle;
    TaskHandle_t current_task_handle;
    BaseType_t operation_status = pdFAIL;
    mutex_t *pMutex;

    if (MutexId > FREERTOS_MAX_SEM_AND_MTX_NUM)
    {
        osif_ret_code = 1;
    }
    else
    {
        pMutex = SemphrTbl[MutexId];
        if (pMutex != NULL)
        {
            /* Two dependencies for FreeRTOS config file */
            /* INCLUDE_xQueueGetMutexHolder */
            /* INCLUDE_xTaskGetCurrentTaskHandle */
            mutex_holder_handle = xSemaphoreGetMutexHolder(*pMutex);
            current_task_handle = xTaskGetCurrentTaskHandle();

            /* If pMutex has been locked by current task, return error. */
            if (mutex_holder_handle == current_task_handle)
            {
                osif_ret_code = 1;
            }
            else
            {
                /* Convert timeout from millisecond to tick. */
                if (timeout == OSIF_WAIT_FOREVER)
                {
                    timeoutTicks = portMAX_DELAY;
                }
                else
                {
                    timeoutTicks = MSEC_TO_TICK(timeout);
                }

                /* Try to take the semaphore */
                operation_status = xSemaphoreTake(*pMutex, timeoutTicks);

                osif_ret_code = (operation_status == pdPASS) ? 0 : 1;
            }
        }
    }

    return osif_ret_code;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : OSIF_MutexUnlock
 * Description   : This function unlocks the mutex, fails if the current thread
 *  is not the mutex holder.
 *
 * Implements : OSIF_MutexUnlock_freertos_Activity
 *END**************************************************************************/
u32 OSIF_MutexUnlock(u8 MutexId)
{
    /* The (pMutex == NULL) case is a valid option, signaling that the mutex does
     * not need to be unlocked - do not use DEV_ASSERT in this case */

    u32 osif_ret_code = 0;
    TaskHandle_t mutex_holder_handle;
    TaskHandle_t current_task_handle;
    BaseType_t operation_status = pdFAIL;
    mutex_t *pMutex;

    if (MutexId > FREERTOS_MAX_SEM_AND_MTX_NUM)
    {
        osif_ret_code = 1;
    }
    else
    {
        pMutex = SemphrTbl[MutexId];
        if (pMutex != NULL)
        {
            /* Two dependencies for FreeRTOS config file */
            /* INCLUDE_xQueueGetMutexHolder */
            /* INCLUDE_xTaskGetCurrentTaskHandle */
            mutex_holder_handle = xSemaphoreGetMutexHolder(*pMutex);
            current_task_handle = xTaskGetCurrentTaskHandle();

            /* If pMutex is not locked by current task, return error. */
            if (mutex_holder_handle != current_task_handle)
            {
                osif_ret_code = 1;
            }
            else
            {
                operation_status = xSemaphoreGive(*pMutex);
                osif_ret_code = (operation_status == pdPASS) ? 0 : 1;
            }
        }
    }

    return osif_ret_code;
}

#if 0
/*FUNCTION**********************************************************************
 *
 * Function Name : OSIF_MutexCreate
 * Description   : This function creates (registers) a mutex object to the OS.
 *
 * Implements : OSIF_MutexCreate_freertos_Activity
 *END**************************************************************************/
u32 OSIF_MutexCreate(mutex_t * const pMutex)
{
    /* The (pMutex == NULL) case is a valid option, signaling that the mutex does
     * not need to be created - do not use DEV_ASSERT in this case */

    u32 osif_ret_code = 0;

    if (pMutex != NULL)
    {
        *pMutex = xSemaphoreCreateMutex();
        if (*pMutex == NULL)
        {
            osif_ret_code = 1; /* mutex not created successfully */
        }
    }

    return osif_ret_code;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : OSIF_MutexDestroy
 * Description   : This function removes the mutex from the OS (and frees memory).
 *
 * Implements : OSIF_MutexDestroy_freertos_Activity
 *END**************************************************************************/
u32 OSIF_MutexDestroy(const mutex_t * const pMutex)
{
    /* The (pMutex == NULL) case is a valid option, signaling that the mutex does
     * not need to be destroyed - do not use DEV_ASSERT in this case */

    if (pMutex != NULL)
    {
        DEV_ASSERT(*pMutex);
        vSemaphoreDelete(*pMutex);
    }

    return 0;
}
#endif

/*FUNCTION**********************************************************************
 *
 * Function Name : OSIF_SemaWait
 * Description   : This function performs the 'wait' (decrement) operation on a semaphore,
 *  returns error if operation times out.
 *
 * Implements : OSIF_SemaWait_freertos_Activity
 *END**************************************************************************/
u32 OSIF_SemaWait(const u8 SemId, const u32 timeout)
{
    u32 timeoutTicks;
    BaseType_t operation_status;
    u32 osif_ret_code;
    semaphore_t *pSem;

    if (SemId > FREERTOS_MAX_SEM_AND_MTX_NUM)
    {
        osif_ret_code = 1;
    }
    else
    {
        pSem = SemphrTbl[SemId];
        /* Convert timeout from millisecond to ticks. */
        if (timeout == OSIF_WAIT_FOREVER)
        {
            timeoutTicks = portMAX_DELAY;
        }
        else
        {
            timeoutTicks = MSEC_TO_TICK(timeout);
        }

        /* Try to take the semaphore */
        operation_status = xSemaphoreTake(*pSem, timeoutTicks);

        osif_ret_code = (operation_status == pdPASS) ? 0 : 1;
    }

    return osif_ret_code;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : OSIF_SemaPost
 * Description   : This function performs the 'post' (increment) operation on a semaphore.
 *
 * Implements : OSIF_SemaPost_freertos_Activity
 *END**************************************************************************/
u32 OSIF_SemaPost(const u8 SemId)
{
    BaseType_t operation_status = pdFAIL;
    u32 osif_ret_code;
    semaphore_t *pSem;
    bool is_isr;

    if (SemId > FREERTOS_MAX_SEM_AND_MTX_NUM)
    {
        osif_ret_code = 1;
    }
    else
    {
        pSem = SemphrTbl[SemId];
        /* Check if the post operation is executed from ISR context */
        is_isr = osif_IsIsrContext();
        if (is_isr)
        {
            /* Execution from exception handler (ISR) */
            BaseType_t taskWoken = pdFALSE;
            operation_status = xSemaphoreGiveFromISR(*pSem, &taskWoken);

            if (operation_status == pdPASS)
            {
                /* Perform a context switch if necessary */
                portYIELD_FROM_ISR(taskWoken);
            }
        }
        else
        {
            /* Execution from task */
            operation_status = xSemaphoreGive(*pSem);
        }

        /* pdFAIL in case that the semaphore is full */
        osif_ret_code = (operation_status == pdPASS) ? 0 : 1;
    }

    return osif_ret_code;
}

#if 0
/*FUNCTION**********************************************************************
 *
 * Function Name : OSIF_SemaCreate
 * Description   : This function creates (registers) a semaphore object to the OS.
 *
 * Implements : OSIF_SemaCreate_freertos_Activity
 *END**************************************************************************/
u32 OSIF_SemaCreate(semaphore_t * const pSem, const u32_t initValue)
{
    DEV_ASSERT(pSem);

    u32 osif_ret_code = 0;

    *pSem = xSemaphoreCreateCounting(0xFFu, initValue);

    if (*pSem == NULL)
    {
        osif_ret_code = 1; /* semaphore not created successfully */
    }

    return osif_ret_code;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : OSIF_SemaDestroy
 * Description   : This function removes a semaphore object from the OS (frees memory).
 *
 * Implements : OSIF_SemaDestroy_freertos_Activity
 *END**************************************************************************/
u32 OSIF_SemaDestroy(const semaphore_t * const pSem)
{
    DEV_ASSERT(pSem);
    DEV_ASSERT(*pSem);

    vSemaphoreDelete(*pSem);

    return 0;
}
#endif

u8 OSIF_GetMailboxPool(u8 tskId)
{
    return (tskId);
}

u32 OSIF_ActivateTsk(u8 tskId)
{
    TaskHandle_t xHandle = NULL;
    bool is_isr;
    u32 osif_ret_code = 0;

    if (tskId > FREERTOS_MAX_TASK_NUM)
    {
        osif_ret_code = 1;
    }
    else
    {
        //xHandle = xTaskGetCurrentTaskHandle();
        xHandle = TaskTcbTbl[tskId];
        if (xHandle != NULL)
        {
            /* Check if the post operation is executed from ISR context */
            is_isr = osif_IsIsrContext();
            if (is_isr)
            {
                /* Execution from exception handler (ISR) */
                portBASE_TYPE xYieldRequired;

                xYieldRequired = xTaskResumeFromISR(xHandle);
                if (xYieldRequired == pdPASS)
                {
                    /* Perform a context switch if necessary */
                    portYIELD_FROM_ISR(xYieldRequired);
                }
            }
            else
            {
                vTaskResume(xHandle);
            }
        }
        else
        {
            osif_ret_code = 1;
        }
    }

    return osif_ret_code;
}

u32 OSIF_TerminateTsk(u8 tskId)
{
    TaskHandle_t xHandle = NULL;
    u32 osif_ret_code = 0;

    if (tskId > FREERTOS_MAX_TASK_NUM)
    {
        osif_ret_code = 1;
    }
    else
    {
        //xHandle = xTaskGetCurrentTaskHandle();
        xHandle = TaskTcbTbl[tskId];
        if (xHandle != NULL)
        {
            vTaskSuspend(xHandle);
        }
        else
        {
            osif_ret_code = 1;
        }
    }

    return osif_ret_code;
}

u8 OSIF_GetActiveTask(void)
{
    TaskHandle_t xHandle = NULL;
    u8 TaskIdCnt = 0;

    xHandle = xTaskGetCurrentTaskHandle();
    for (TaskIdCnt = 1; TaskIdCnt < FREERTOS_MAX_TASK_NUM; TaskIdCnt++)
    {
        if (TaskTcbTbl[TaskIdCnt] == xHandle)
        {
            break;
        }
    }

    if (TaskIdCnt == FREERTOS_MAX_TASK_NUM)
    {
        TaskIdCnt = 0xff;
    }

    return TaskIdCnt;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : OSIF_EventPoolInit
 * Description   : This function init event pool.
 *
 * Implements :
 *END**************************************************************************/
void OSIF_EventPoolInit(void)
{
}

u32 OSIF_EVSendTask(u8 tskId, s32 event)
{
    u32 RetVal = 1;
    bool is_isr;
    BaseType_t xHigherPriorityTaskWoken;
    QueueHandle_t xQueue;
    BaseType_t xYieldRequired;

    if (tskId > FREERTOS_MAX_TASK_NUM)
    {
        RetVal = 2;
    }

    xQueue = TaskDataQueueTbl[tskId];
    if (xQueue != NULL)
    {
        is_isr = osif_IsIsrContext();
        if (is_isr)
        {
            xYieldRequired = xQueueSendFromISR(xQueue, (void *)&event, &xHigherPriorityTaskWoken);
            if (xHigherPriorityTaskWoken)
            {
                // Actual macro used here is port specific.
                portYIELD_FROM_ISR(xYieldRequired);
            }
        }
        else
        {
            xQueueSend(xQueue, (void *)&event, 5);
        }
    }
    else
    {
        RetVal = 3;
    }

    return RetVal;
}

u32 OSIF_WaitEvent(u32 inEvents, u32 timeout)
{
    u32 singal = 0;
    u8 tskId;
    BaseType_t RetVal = pdPASS;
    QueueHandle_t xQueue = NULL;

    tskId = OSIF_GetActiveTask();
    xQueue = TaskDataQueueTbl[tskId];
    if (xQueue != NULL)
    {
        if (timeout == OS_DONOTWAIT)
        {
            RetVal = xQueueReceive(xQueue, (u32 *)&singal, 0);
        }
        else
        {
            if (timeout == OS_WAITFOREVER)
            {
                RetVal = xQueueReceive(xQueue, (u32 *)&singal, portMAX_DELAY);
            }
            else
            {
                RetVal = xQueueReceive(xQueue, (u32 *)&singal, (TickType_t)timeout);
            }
        }

        if (pdPASS == RetVal)
        {
            if ((inEvents | EVENT_GLOBAL_MAILBOX) & singal)
            {
                //return events;
            }
            else
            {
                singal = 0x00;
            }
        }
        else
        {
            singal = 0x00;
        }
    }
    else
    {
    }

    return (singal);
}

static u32 OSIF_SendMailboxInternal(u8 mbid, SMP_MSG *mb_msg)
{
    u32 RetVal = 0;
    bool is_isr;
    BaseType_t xHigherPriorityTaskWoken;
    QueueHandle_t xQueue;
    BaseType_t xYieldRequired;

    if (mbid > FREERTOS_MAX_TASK_NUM)
    {
        RetVal = 1;
    }

    xQueue = TaskDataQueueTbl[mbid];
    if (xQueue != NULL)
    {
        is_isr = osif_IsIsrContext();
        if (is_isr)
        {
            xYieldRequired = xQueueSendFromISR(xQueue, (void *)&mb_msg, &xHigherPriorityTaskWoken);
            if (xHigherPriorityTaskWoken)
            {
                // Actual macro used here is port specific.
                portYIELD_FROM_ISR(xYieldRequired);
            }
        }
        else
        {
            xQueueSend(xQueue, (void *)&mb_msg, 100);
        }
    }
    else
    {
        RetVal = 2;
    }

    return RetVal;
}

/**
*   \brief   for internal use :receive msg per mailbox
*   \param
*   \return E_OK if success, err code if fail
*
*/
s32 OS_ReceiveMailboxInternal(u8 mbid, SMP_MSG **mb_msg, u32 timeout)
{
    BaseType_t RetVal = pdPASS;
    QueueHandle_t xQueue = NULL;

    xQueue = TaskDataQueueTbl[mbid];
    if (xQueue != NULL)
    {
        if (timeout == OS_DONOTWAIT)
        {
            RetVal = xQueueReceive(xQueue, (u32 *)&mb_msg, 0);
        }
        else
        {
            if (timeout == OS_WAITFOREVER)
            {
                RetVal = xQueueReceive(xQueue, (u32 *)&mb_msg, portMAX_DELAY);
            }
            else
            {
                RetVal = xQueueReceive(xQueue, (u32 *)&mb_msg, (TickType_t)timeout);
            }
        }

        if (pdPASS != RetVal)
        {
            mb_msg = NULL;
            RetVal = 1;
        }
        else
        {
            RetVal = 0;
        }
    }
    else
    {
    }

    return (RetVal);
}

u32 OSIF_SendMailbox(u8 mbid, u8 *pcMsg, u16 u16MsgLen)
{
    u32 RetVal = 0;
    SMP_MSG *mqMsg = NULL;
    u8 *pData = NULL;
    u16 MemoryLen;
    bool is_isr;

    if ((NULL != pcMsg) && (u16MsgLen > 0))
    {
        is_isr = osif_IsIsrContext();
        if (is_isr == 0)
        {
            MemoryLen = (u16MsgLen + 3) & 0xfffc;
            MemoryLen = MemoryLen + sizeof(SMP_MSG);

            pData = pvPortMalloc(MemoryLen);
            if (pData != NULL)
            {
                mqMsg = (SMP_MSG *)pData;
                mqMsg->datalen = u16MsgLen;
                pData += sizeof(SMP_MSG);

                memcpy(pData, pcMsg, u16MsgLen);

                RetVal = OSIF_SendMailboxInternal(mbid, mqMsg);
                if (RetVal != 0)
                {
                    vPortFree(pData);
                }
            }
            else
            {
                RetVal = 1;
            }
        }
        else
        {
            RetVal = 2;
        }
    }
    else
    {
        RetVal = 3;
    }

    return RetVal;
}

/**
*   \brief   receive msg per mailbox ID
*   \param
*   \return E_OK if success, err code if fail
*
*/
u32 OSIF_ReceiveMailbox(u8 mbid, u8 **pBuf, u16 *pBufLen, u32 timeout)
{
    u32 RetVal = 0;
    SMP_MSG *mqMsg;
    u8 *pData = NULL;

    if ((NULL != pBuf) && (pBufLen != NULL))
    {
        RetVal = OS_ReceiveMailboxInternal((u8)mbid, &mqMsg, (u32)timeout);
        if (0 == RetVal)
        {
            pData = (u8 *)mqMsg;
            pData += sizeof(SMP_MSG);

            *pBuf = pData;
            *pBufLen = mqMsg->datalen;
        }
    }
    else
    {
        RetVal = 1;
    }

    return RetVal;
}

/**/
u32 OSIF_MQSend(u8 tskId, u8 *pcMsg, u16 u16MsgLen)
{
    u32 RetVal = 0;
    u8 mbxId;
    bool is_isr;

    if ((tskId <= FREERTOS_MAX_TASK_NUM) && (NULL != pcMsg) && (u16MsgLen > 0))
    {
        is_isr = osif_IsIsrContext();
        if (is_isr == 1)
        {
            RetVal = 1;
        }
        else
        {
            mbxId = tskId;
            if (0 == OSIF_SendMailbox(mbxId, pcMsg, u16MsgLen))
            {
                OSIF_EVSendTask(tskId, EVENT_GLOBAL_MAILBOX);
            }
            else
            {
                RetVal = 2;
            }
        }
    }
    else
    {
        RetVal = 3;
    }

    return RetVal;
}

void OS_HOOK_ErrorHook(u8 State, u8 ServiceId, u8 ID)
{
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
