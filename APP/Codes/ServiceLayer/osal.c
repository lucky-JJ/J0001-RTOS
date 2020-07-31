
/* --- include files ------------------------------------------------------ */

//#include "OSIF.h"
#include "osal.h"
#include "EventDefine.h"
//#include "Alarm.h"
#include "osif_freertos.h"

#include "cychdr.h"
#include "app.h"

/*---------------------------------------------------------------------------
 * VARIABLES
 *---------------------------------------------------------------------------*/
/*  0.声明2个任务项目的函数指针 */
#undef _TSK_CFG_
#undef STDTASKDEF
#define STDTASKDEF(osName, Prio, StackDepth) \
    static TCfTaskFunc pFunc##osName;        \
    static TCfHandleMsgFunc pMsgFunc##osName;
#include "tsk.h"
#undef STDTASKDEF

u32 cfThreadInit(void)
{
    u32 RetVal = 0;

    /*
        1.初始化任务相关的2个函数指针
    */
#undef _TSK_CFG_
#undef STDTASKDEF
#define STDTASKDEF(osName, Prio, StackDepth) \
    pFunc##osName = NULL;                    \
    pMsgFunc##osName = NULL;
#include "tsk.h"
#undef STDTASKDEF

    /*
        2.注册2个任务相关的函数指针
        因为app.h中定义了 
        #define TASK_CREATE(TaskName)  void TaskName##_Create(void)
		故在预编译时候已经替换掉了,下面直接调用替换之后的
    
        TASK_CREATE(PwrManage) --> PwrManage_Create()
        进入cfThreadCreate()中注册进程,会加载
        pFunc##osName = func;                  
        pMsgFunc##osName = msgFunc;  


        初始化 TASK_CREATE(PwrManage)
    */
#undef _TSK_CFG_
#undef STDTASKDEF
#define STDTASKDEF(osName, Prio, StackDepth) \
    osName##_Create();
#include "tsk.h"
#undef STDTASKDEF

    /*
        3.以上相关任务都已注册完成 , 开始创建任务跑循环

        执行 
    */
    RetVal = OSIF_Init();

    return RetVal;
}

void cfThreadCreate(u8 tskId, TCfTaskFunc func, TCfHandleMsgFunc msgFunc)
{
#undef _TSK_CFG_
#undef STDTASKDEF
#define STDTASKDEF(osName, Prio, StackDepth) \
    if (TID_##osName == tskId)               \
    {                                        \
        pFunc##osName = func;                \
        pMsgFunc##osName = msgFunc;          \
    }
#include "tsk.h"
#undef STDTASKDEF
}

u32 cfThreadWaitWithTimeout(u32 signals, u16 timeout)
{
    u32 event;
    u8 tskId;
    u8 *pBuf = NULL;
    u16 BufLen;

    tskId = OSIF_GetActiveTask();

    event = OSIF_WaitEvent((EVENT_GLOBAL_MAILBOX | signals), timeout);

    if (event & EVENT_GLOBAL_MAILBOX)
    {
        u8 mbxId = OSIF_GetMailboxPool(tskId);

        if (0 == OSIF_ReceiveMailbox(mbxId, &pBuf, &BufLen, OS_DONOTWAIT))
        {
            switch (tskId)
            {
#undef _TSK_CFG_
#undef STDTASKDEF
#define STDTASKDEF(osName, Prio, StackDepth) \
    case (TID_##osName):                     \
    {                                        \
        if (pMsgFunc##osName)                \
        {                                    \
            pMsgFunc##osName(pBuf, BufLen);  \
        }                                    \
        break;                               \
    }
#include "tsk.h"
#undef STDTASKDEF
            default:
                break;
            }
            OSIF_FreeMailBoxMemory(pBuf);
        }
    }

    return event;
}

#undef _TSK_CFG_
#undef STDTASKDEF
/*---------------------------------------------------------------------------
 * Task functions
 *---------------------------------------------------------------------------*/
#define STDTASKDEF(osName, Prio, StackDepth) \
    void osName##_Task(u32 exinf)            \
    {                                        \
        osName##_Init();                     \
                                             \
        if (NULL != pFunc##osName)           \
        {                                    \
            MAIN_LOOP                        \
            pFunc##osName();                 \
            MAIN_LOOP_END                    \
        }                                    \
    }
#include "tsk.h"
#undef STDTASKDEF

/*rtos 系统中断触发的钩子函数 , 1ms一次*/
void vApplicationTickHook(void)
{
    static u8 cnt = 0;

    if (++cnt == 5)
    {
        cnt = 0;
        cycleTaskTick(); /*5ms 进一次*/
    }
}
