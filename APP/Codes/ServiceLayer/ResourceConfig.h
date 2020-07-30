/*
 * @Author: J
 * @Date: 2020-07-28 10:59:07
 * @LastEditTime: 2020-07-30 18:09:42
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \APP\Codes\ServiceLayer\ResourceConfig.h
 */
#ifndef RESOURCE_CONFIG_H
#define RESOURCE_CONFIG_H

#include "Os.h"
//#include "Swc.h"

typedef void (*TCfTaskFunc)(void);
typedef void (*TCfHandleMsgFunc)(void *pParam, u16 Len);

enum
{
    /* os task id config */
    TID_Uart1 = 1,
    TID_Key,
    TID_Led,
    TID_Lcd,
    TID_Max,

    /* semaphore id config  */

    /*Data Queue ID*/
    DID_Uart = 1,
    DID_Key,
    DID_Led,
    DID_Lcd,
    DID_Max,

    /*MailBox ID*/

    /*Mutex ID*/
};

/* semaphore id config  */
#define SID_RtcMutex (0)
#define SID_I2C0Mutex (1)
#define SID_DbgMutex (2)
#define SID_I2C0Sem (3)

#define MAX_SEM_AND_MTX_NUM (4)

/* event config */

#define EVENT_GLOBAL_MAILBOX (1 << 0)       // for All Task
#define EVENT_GLOBAL_Watchdog (1 << 1)      // for All Task
#define EVT_ID_All_Task_TIMER_5MS (1 << 2)  // for All Task
#define EVT_ID_All_Task_TIMER_10MS (1 << 3) // for All Task

#define EVT_ID_CanTask_COM_UPDATE (1 << 0) // for CanTask
#define EVT_ID_CanTask_TIMER_5MS (1 << 1)  // for CanTask

#define EVT_ID_BswTask_COM_UPDATE (1 << 0) // for BswTask
#define EVT_ID_BswTask_TIMER_5MS (1 << 1)  // for BswTask

#define EVT_ID_MiscTask_COM_UPDATE (1 << 0) // for MiscTask
#define EVT_ID_MiscTask_TIMER_10MS (1 << 1) // for MiscTask

#define EVT_ID_AudioTask_COM_UPDATE (1 << 0) // for AudioTask
#define EVT_ID_AudioTask_TIMER_10MS (1 << 1) // for AudioTask

#define EVT_ID_ComTask_COM_UPDATE (1 << 0)     // for ComTask
#define EVT_ID_ComTask_TIMER_10MS (1 << 1)     // for ComTask
#define EVT_ID_ComTask_UART_TX_FINISH (1 << 2) // for ComTask
#define EVT_ID_SOC_PDU_UART_TX_FINISH (1 << 2) // for ComTask

#define EVT_ID_SecTask_COM_UPDATE (1 << 0) // for SecTask
#define EVT_ID_SecTask_TIMER_20MS (1 << 1) // for SecTask

/* module(swc) id config */
#define SWC_ID_AirCtl (0)  // for CanTask
#define SWC_ID_Avm (1)     // for CanTask
#define SWC_ID_BCM (2)     // for CanTask
#define SWC_ID_Tbox (3)    // for CanTask
#define SWC_ID_CAN_PDU (4) // for CanTask

#define SWC_ID_PwrMgr (0) // for BswTask
#define SWC_ID_BswSrv (1) // for BswTask

#define SWC_ID_Time (0)   // for MiscTask
#define SWC_ID_KeyApp (1) // for MiscTask

#define SWC_ID_Audio (0) // for AudioTask

#define SWC_ID_SOC_PDU (0) // for ComTask

#define SWC_ID_Security (0) // for SecTask

/* module(swc) count */
#define CanTask_ModuleNumber (5)   // for CanTask
#define BswTask_ModuleNumber (2)   // for BswTask
#define MiscTask_ModuleNumber (2)  // for MiscTask
#define AudioTask_ModuleNumber (1) // for AudioTask
#define ComTask_ModuleNumber (1)   // for ComTask
#define SecTask_ModuleNumber (1)   // for SecTask

#define TASK_NUM (6)
#define MAX_SWC_NUM (12)
#define MAX_EVENT_NUM (3)

#if (MAX_SEM_AND_MTX_NUM > 0)
extern SemaphoreHandle_t SemphrTbl[MAX_SEM_AND_MTX_NUM];
#endif

typedef struct
{
    uint8_t SwcNum;
    uint8_t MaskSwcGrp[MAX_SWC_NUM];
} SwcEventMask_t;

void SwcGroupInit(void);
uint32_t TasksStaticCreate(void);
uint32_t SemphrCreateStatic(void);

#endif
