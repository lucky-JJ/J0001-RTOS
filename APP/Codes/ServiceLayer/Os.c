/*
 * @Author: your name
 * @Date: 2020-07-28 09:50:44
 * @LastEditTime: 2020-07-31 18:05:52
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \J0001-RTOS\APP\Codes\ServiceLayer\Os.c
 */
#include "Os.h"
#include "ResourceConfig.h"

#define MSEC_TO_TICK(msec) (pdMS_TO_TICKS(msec))

extern void vPortSetupTimerInterrupt(void);

extern void vPortStopTimerInterrupt(void);

extern bool IsIsrContext(void); //上下文切换检测
/*FUNCTION**********************************************************************
 *
 * Function Name : OSIF_TimeDelay
 * Description   : This function blocks (sleep) the current thread for a number
 *  of milliseconds.
 *
 * Implements : OSIF_TimeDelay_freertos_Activity
 *END**************************************************************************/
void Os_TimeDelay(uint32_t delay)
{
    /* One dependency for FreeRTOS config file */
    /* INCLUDE_vTaskDelay */
    vTaskDelay(MSEC_TO_TICK(delay));
}

void Os_StartTick(void)
{
    vPortSetupTimerInterrupt();
}

void Os_StopTick(void)
{
    vPortStopTimerInterrupt();
}

/*FUNCTION**********************************************************************
 *
 * Function Name : Os_GetMilliseconds
 * Description   : This function returns the number of miliseconds elapsed since
 *                  starting the u32ernal timer (since scheduler was started).
 *
 * Implements : Os_GetMilliseconds_freertos_Activity
 *END**************************************************************************/
uint32_t Os_GetMilliseconds(void)
{
    /*
     * Return the tick count in miliseconds
     * Note: if configTICK_RATE_HZ is less than 1000, the return value will be truncated
     * to 32-bit wide for large values of the tick count.
     */
    return (uint32_t)((((uint64_t)xTaskGetTickCount()) * 1000u) / configTICK_RATE_HZ);
}

uint32_t EvtGroupsNotifyTask(const unsigned char TaskId, const uint32_t NotifyBits)
{
    BaseType_t Ret;

    Ret = xTaskNotify((TaskHandle_t)TaskList[TaskId], NotifyBits, eSetBits);
    if (pdPASS == Ret)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

uint32_t EvtGroupsNotifyTaskFromISR(const unsigned char TaskId, const uint32_t NotifyBits)
{
    BaseType_t Ret;

    Ret = xTaskNotifyFromISR((TaskHandle_t)TaskList[TaskId], NotifyBits, eSetBits, NULL);
    if (pdPASS == Ret)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

uint32_t EvtGroupsWaitBits(const unsigned char TaskId, const uint32_t BitsToWaitFor)
{
    uint32_t Bits = 0;
    uint32_t ulNotifiedValue;
    BaseType_t Ret;

    (void)TaskId;
    Ret = xTaskNotifyWait(0x00, 0xffffffffUL, &ulNotifiedValue, portMAX_DELAY);
    if (Ret == pdTRUE)
    {
        if ((ulNotifiedValue & BitsToWaitFor) != 0)
        {
            Bits = ulNotifiedValue;
        }
    }

    return Bits;
}

/**
 * @description: OS 定时器回调函数,1ms一次
 * @param {type} 
 * @return: 
 */
void vApplicationTickHook(void)
{
    Alarm_Counter(1);
}

void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
    (void)pcTaskName;
    (void)pxTask;

    /* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected.  This function is
	provided as an example only as stack overflow checking does not function
	when running the FreeRTOS Windows port. */
    //vAssertCalled( __LINE__, __FILE__ );
}

void vApplicationMallocFailedHook(void)
{
    /* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
    taskDISABLE_INTERRUPTS();
    for (;;)
        ;
}

void vApplicationIdleHook(void)
{
    /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */
}

static StaticTask_t xIdleTaskTCB;
static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize)
{
    /* If the buffers to be provided to the Idle task are declared inside this
function then they must be declared static - otherwise they will be allocated on
the stack and so not exists after this function exits. */

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

uint8_t EventSendTask(unsigned char Taskid, uint32_t Event)
{
    uint8_t Ret = 0;

    EvtGroupsNotifyTask(Taskid, Event);
    return Ret;
}

uint8_t EventSendTaskFormISR(unsigned char Taskid, uint32_t Event)
{
    uint8_t Ret = 0;

    EvtGroupsNotifyTaskFromISR(Taskid, Event);
    return Ret;
}

void OS_HOOK_ErrorHook(uint8_t State, uint8_t ServiceId, uint8_t ID)
{
}

u32 Os_MtxLock(u8 MutexId, const u32 timeout)
{
    u32 timeoutTicks;
    u32 osif_ret_code = 1;
    TaskHandle_t mutex_holder_handle;
    TaskHandle_t current_task_handle;
    BaseType_t operation_status = pdFAIL;
    mutex_t *pMutex;

    if (MutexId > MAX_SEM_AND_MTX_NUM)
    {
        osif_ret_code = 1;
    }
    else
    {
        pMutex = (mutex_t *)SemphrTbl[MutexId];
        if (pMutex != NULL)
        {
            /* Two dependencies for FreeRTOS config file */
            /* INCLUDE_xQueueGetMutexHolder */
            /* INCLUDE_xTaskGetCurrentTaskHandle */
            mutex_holder_handle = xSemaphoreGetMutexHolder((QueueHandle_t)pMutex);
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
                operation_status = xSemaphoreTake((QueueHandle_t)pMutex, timeoutTicks);

                osif_ret_code = (operation_status == pdPASS) ? 0 : 1;
            }
        }
    }

    return osif_ret_code;
}

/**
 * @description: 互斥量释放
 * @param {type} 
 * @return: 
 */
u32 Os_MtxUnlock(u8 MutexId)
{
    /* The (pMutex == NULL) case is a valid option, signaling that the mutex does
     * not need to be unlocked - do not use DEV_ASSERT in this case */

    u32 osif_ret_code = 0;
    TaskHandle_t mutex_holder_handle;
    TaskHandle_t current_task_handle;
    BaseType_t operation_status = pdFAIL;
    mutex_t *pMutex;

    if (MutexId > MAX_SEM_AND_MTX_NUM)
    {
        osif_ret_code = 1;
    }
    else
    {
        pMutex = (mutex_t *)SemphrTbl[MutexId];
        if (pMutex != NULL)
        {
            /* Two dependencies for FreeRTOS config file */
            /* INCLUDE_xQueueGetMutexHolder */
            /* INCLUDE_xTaskGetCurrentTaskHandle */
            mutex_holder_handle = xSemaphoreGetMutexHolder((QueueHandle_t)pMutex);
            current_task_handle = xTaskGetCurrentTaskHandle();

            /* If pMutex is not locked by current task, return error. */
            if (mutex_holder_handle != current_task_handle)
            {
                osif_ret_code = 1;
            }
            else
            {
                operation_status = xSemaphoreGive(pMutex);
                osif_ret_code = (operation_status == pdPASS) ? 0 : 1;
            }
        }
    }

    return osif_ret_code;
}

u32 Os_SemWait(const u8 SemId, const u32 timeout)
{
    u32 timeoutTicks;
    BaseType_t operation_status;
    u32 osif_ret_code;
    semaphore_t *pSem;

    if (SemId > MAX_SEM_AND_MTX_NUM)
    {
        osif_ret_code = 1;
    }
    else
    {
        pSem = (semaphore_t *)SemphrTbl[SemId];
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
        operation_status = xSemaphoreTake((QueueHandle_t)pSem, timeoutTicks);

        osif_ret_code = (operation_status == pdPASS) ? 0 : 1;
    }

    return osif_ret_code;
}

/**
 * @description: 
 * @param {type} 
 * @return: 
 */
u32 Os_SemPost(const u8 SemId)
{
    BaseType_t operation_status = pdFAIL;
    u32 osif_ret_code;
    semaphore_t *pSem;
    bool is_isr;

    if (SemId > MAX_SEM_AND_MTX_NUM)
    {
        osif_ret_code = 1;
    }
    else
    {
        pSem = (semaphore_t *)SemphrTbl[SemId];
        /* Check if the post operation is executed from ISR context */
        is_isr = IsIsrContext();
        if (is_isr)
        {
            /* Execution from exception handler (ISR) */
            BaseType_t taskWoken = pdFALSE;
            operation_status = xSemaphoreGiveFromISR(pSem, &taskWoken);

            if (operation_status == pdPASS)
            {
                /* Perform a context switch if necessary */
                portYIELD_FROM_ISR(taskWoken);
            }
        }
        else
        {
            /* Execution from task */
            operation_status = xSemaphoreGive(pSem);
        }

        /* pdFAIL in case that the semaphore is full */
        osif_ret_code = (operation_status == pdPASS) ? 0 : 1;
    }

    return osif_ret_code;
}

void ResumeAllInterrupts(void)
{
    taskENABLE_INTERRUPTS();
}

void SuspendAllInterrupts(void)
{
    taskDISABLE_INTERRUPTS();
}

/**
 * @description: 调度锁开启
 * @param {type} 
 * @note:   1. 调度锁函数只是禁止了任务调度，并没有关闭任何中断。
            2. 调度锁开启函数 vTaskSuspendAll 和调度锁关闭函数 xTaskResumeAll 一定要成对使用。
            3. 切不可在调度锁开启函数 vTaskSuspendAll 和调度锁关闭函数 xTaskResumeAll 之间调用任何会引
            起任务切换的 API，比如 vTaskDelayUntil、vTaskDelay、xQueueSend 等
 * @return: 
 */
void Os_TaskSuspendAll(void)
{
    vTaskSuspendAll();
}
/**
 * @description: 调度锁关闭
 * @param {type} 
 * @return: 
 */
void Os_TaskResumeAll(void)
{
    xTaskResumeAll();
}

const char *Os_GetCurTaskName(void)
{
    return (const char *)pcTaskGetName(xTaskGetCurrentTaskHandle());
}

/**
*   \brief   for internal use :receive msg per mailbox
*   \param
*   \return E_OK if success, err code if fail
*
*/
s32 OS_ReceiveMailboxInternal(u8 mbid, SMP_MSG **mb_msg, u32 timeout)
{
#if (USE_TASK_MAIL_BOX_LIST == 1)
    u32 RetVal = 0;
    List_t *pList;
    ListItem_t *pxIterator;

    (void)timeout;

    if (mbid > FREERTOS_MAX_TASK_NUM || mb_msg == NULL)
    {
        RetVal = 1;
    }
    else
    {
        pList = &TaskMailBoxTbl[mbid];
        if (listLIST_IS_EMPTY(pList))
        {
            RetVal = 2;
        }
        else
        {
            pxIterator = (ListItem_t *)&(pList->xListEnd);
            if (0 < pList->uxNumberOfItems)
            {
                *mb_msg = (SMP_MSG *)pxIterator->pxNext->xItemValue;
            }
            else
            {
                *mb_msg = NULL;
            }
        }
    }

    return (RetVal);
#else
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
#endif
}

/**
*   \brief   receive msg per mailbox ID
*   \param
*   \return E_OK if success = 0, err code if fail = 1
*
*/
u32 OS_ReceiveMailbox(u8 mbid, u8 **pBuf, u16 *pBufLen, u32 timeout)
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

static u32 OS_SendMailboxInternal(u8 mbid, SMP_MSG *mb_msg)
{
#if (USE_TASK_MAIL_BOX_LIST == 1)
    u32 RetVal = 0;
    List_t *pList;
    SMP_MSG *mqMsg = NULL;

    if (mbid > FREERTOS_MAX_TASK_NUM || mb_msg == NULL)
    {
        RetVal = 1;
    }
    else
    {
        pList = &TaskMailBoxTbl[mbid];
        mqMsg = (SMP_MSG *)mb_msg;

        vTaskSuspendAll();
        vListInsertEnd(pList, &(mqMsg->Item));
        (void)xTaskResumeAll();
    }

    return RetVal;
#else
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
        is_isr = IsIsrContext();
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
#endif
}

u32 OS_SendMailbox(u8 mbid, u8 *pcMsg, u16 u16MsgLen)
{
    u32 RetVal = 0;
    SMP_MSG *mqMsg = NULL;
    u8 *pData = NULL;
    u16 MemoryLen;
    bool is_isr;

    if ((NULL != pcMsg) && (u16MsgLen > 0))
    {
        is_isr = IsIsrContext();
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

#if (USE_TASK_MAIL_BOX_LIST == 1)
                mqMsg->Item.xItemValue = (u32)mqMsg;
#endif

                RetVal = OS_SendMailboxInternal(mbid, mqMsg);
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

u32 OS_MQSend(u8 tskId, u8 *pcMsg, u16 u16MsgLen)
{
    u32 RetVal = 0;
    u8 mbxId;
    bool is_isr;

    if ((tskId <= FREERTOS_MAX_TASK_NUM) && (NULL != pcMsg) && (u16MsgLen > 0))
    {
        is_isr = IsIsrContext();
        if (is_isr == 1)
        {
            RetVal = 1;
        }
        else
        {
            mbxId = tskId;
            if (0 == OS_SendMailbox(mbxId, pcMsg, u16MsgLen))
            {
                EventSendTask(tskId, EVENT_GLOBAL_MAILBOX);
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

void OS_FreeMailBoxMemory(void *pBuf)
{
    SMP_MSG *mqMsg = NULL;
    pBuf = pBuf - sizeof(SMP_MSG);
    mqMsg = (SMP_MSG *)pBuf;

    vTaskSuspendAll();
    uxListRemove(&(mqMsg->Item));
    (void)xTaskResumeAll();

    vPortFree(pBuf);
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

void StartOS(void)
{
    vTaskStartScheduler();
}

/**
 * OS shutdown
 *
 * @param Error - Reason for shutdown
 */

/** @req OS071 */
void ShutdownOS(u8 Error)
{
#if 0
    /** !req OS054 */
    if( OS_SYS_PTR->hooks->ShutdownHook != NULL )
    {
        OS_SYS_PTR->hooks->ShutdownHook(Error);
    }

    Irq_Disable();
#endif
    /** @req OS425 */
    while (1)
    {
        ;
    }
}