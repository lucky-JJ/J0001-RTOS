/*
 * @Author: your name
 * @Date: 2020-05-29 09:49:07
 * @LastEditTime: 2020-07-31 13:17:49
 * @LastEditors: your name
 * @Description: In User Settings Edit
 * @FilePath: \J0001-RTOS\APP\Codes\ServiceLayer\TaskId.h
 */
#ifndef TASK_ID_H
#define TASK_ID_H

#include "sys.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"

#define FREERTOS_MAX_TASK_NUM (3)

/*Data Queue ID*/
#define DID_PwrManage (1)

/*MailBox ID*/
#define MID_PwrManage (1)

#define FREERTOS_MAX_SEM_AND_MTX_NUM (3)

/*Semaphore ID*/
#define ID_AuidoSem (1)

/*Mutex ID*/
#define ID_RtcMtx (2)

extern TaskHandle_t TaskTcbTbl[FREERTOS_MAX_TASK_NUM];
extern QueueHandle_t TaskDataQueueTbl[FREERTOS_MAX_TASK_NUM];
//extern QueueHandle_t TaskMailBoxTbl[FREERTOS_MAX_TASK_NUM];
extern SemaphoreHandle_t SemphrTbl[FREERTOS_MAX_SEM_AND_MTX_NUM];

extern int TasksCreateStatic(void);
extern int DataQueueCreateStatic(void);
extern int SemphrCreateStatic(void);

#endif
