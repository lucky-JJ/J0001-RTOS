#include "TaskId.h"
#include "sys.h"
//#include "EventDefine.h"
//#include "Tmr.h"
//#include "Alarm.h"
//#include "AlarmCfg.h"
#include "osif_freertos.h"

TaskHandle_t TaskTcbTbl[FREERTOS_MAX_TASK_NUM];
QueueHandle_t TaskDataQueueTbl[FREERTOS_MAX_TASK_NUM];
QueueHandle_t TaskMailBoxTbl[FREERTOS_MAX_TASK_NUM];
SemaphoreHandle_t SemphrTbl[FREERTOS_MAX_SEM_AND_MTX_NUM];


/*声明在osal.c中定义的osName##_Task*/
# undef _TSK_CFG_
# undef STDTASKDEF
# define STDTASKDEF(osName, Mode, Prio, stack, dtqdepth)  \
extern void osName##_Task(void *pvParameters);
# include "tsk.h"
# undef STDTASKDEF

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

    xTaskCreate(Uart1_Task, "Uart1", 200, NULL, 1, &xHandle);
    if(xHandle != NULL)
    {
        TaskIDCnt++;
        TaskTcbTbl[TaskIDCnt] = xHandle;
    }
    else
    {
        return 1;
    }

	
    xTaskCreate(Key_Task, "Key", 50, NULL, 2, &xHandle);
    if(xHandle != NULL)
    {
        TaskIDCnt++;
        TaskTcbTbl[TaskIDCnt] = xHandle;
    }
    else
    {
        return 1;
    }

    xTaskCreate(Led_Task, "Led", 50, NULL, 3, &xHandle);
    if(xHandle != NULL)
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
    if(xQueue != NULL)
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

//信号量
int SemphrCreateStatic(void)
{
    SemaphoreHandle_t xSemaphore = NULL;
    u8 SemIDCnt = 0;


    xSemaphore = xSemaphoreCreateCounting(1, 0);
    if(xSemaphore != NULL)
    {
        SemIDCnt++;
        SemphrTbl[SemIDCnt] = xSemaphore;
    }
    else
    {
        return 1;
    }
    xSemaphore = xSemaphoreCreateMutex();
    if(xSemaphore != NULL)
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
