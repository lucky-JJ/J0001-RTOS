#include "TaskId.h"
#include "sys.h"
//#include "EventDefine.h"
//#include "Tmr.h"
//#include "Alarm.h"
//#include "AlarmCfg.h"
#include "osif_freertos.h"

TaskHandle_t TaskTcbTbl[TID_Max];
QueueHandle_t TaskDataQueueTbl[TID_Max];
SemaphoreHandle_t SemphrTbl[TID_Max];

/*声明在osal.c中定义的osName##_Task*/
#undef _TSK_CFG_
#undef STDTASKDEF
#define STDTASKDEF(osName, Prio, StackDepth) \
    extern void osName##_Task(void *pvParameters);
#include "tsk.h"
#undef STDTASKDEF

void SysTicCycleHanldeHook(int Tick)
{
    //    Alarm_Counter(2);
}

//创建任务
int TasksCreateStatic(void)
{
    TaskHandle_t xHandle = NULL;
    u8 TaskIDCnt = 0;

    /*
        开始创建任务 , 任务指针 Key_Task -->            osName##_Task()
        开始跑循环 , 在 osal.c中有定义

		osName##_Task()
		{
			init();
			while(1)
			{
				;
			}
		}
    */

#undef _TSK_CFG_
#undef STDTASKDEF
#define STDTASKDEF(osName, Prio, StackDepth)                                \
    xTaskCreate(osName##_Task, "osName", StackDepth, NULL, Prio, &xHandle); \
    TaskIDCnt++;                                                            \
    TaskTcbTbl[TaskIDCnt] = xHandle;

#include "tsk.h"
#undef STDTASKDEF

    xTaskCreate(Uart1_Task, "Uart1", 200, NULL, 1, &xHandle);
    if (xHandle != NULL)
    {
        TaskIDCnt++;
        TaskTcbTbl[TaskIDCnt] = xHandle;
    }
    else
    {
        return 1;
    }

    xTaskCreate(Key_Task, "Key", 50, NULL, 2, &xHandle);
    if (xHandle != NULL)
    {
        TaskIDCnt++;
        TaskTcbTbl[TaskIDCnt] = xHandle;
    }
    else
    {
        return 1;
    }

    xTaskCreate(Led_Task, "Led", 50, NULL, 3, &xHandle);
    if (xHandle != NULL)
    {
        TaskIDCnt++;
        TaskTcbTbl[TaskIDCnt] = xHandle;
    }
    else
    {
        return 1;
    }

    return 0;
}

//创建队列
int DataQueueCreateStatic(void)
{
    QueueHandle_t xQueue = NULL;
    u8 DataQueueCnt = 0;

    xQueue = xQueueCreate(180, sizeof(u32));
    if (xQueue != NULL)
    {
        DataQueueCnt++;
        TaskDataQueueTbl[DataQueueCnt] = xQueue;
    }
    else
    {
        return 1;
    }

    DataQueueCnt++;
    TaskDataQueueTbl[DataQueueCnt] = NULL;

    return 0;
}

/**
 * @description:创建静态信号量
 * @param {type} 
 * @return: 
 */
int SemphrCreateStatic(void)
{
    SemaphoreHandle_t xSemaphore = NULL;
    u8 SemIDCnt = 0;

    xSemaphore = xSemaphoreCreateCounting(1, 0);
    if (xSemaphore != NULL)
    {
        SemIDCnt++;
        SemphrTbl[SemIDCnt] = xSemaphore;
    }
    else
    {
        return 1;
    }
    xSemaphore = xSemaphoreCreateMutex();
    if (xSemaphore != NULL)
    {
        SemIDCnt++;
        SemphrTbl[SemIDCnt] = xSemaphore;
    }
    else
    {
        return 1;
    }

    return 0;
}
