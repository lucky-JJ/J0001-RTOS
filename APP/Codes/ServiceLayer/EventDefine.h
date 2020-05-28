#ifndef EVENT_DEFINE_H
#define EVENT_DEFINE_H


/*最多支持32个事件 , 其中 1,2 固定了*/

// Events for all tasks:
#define EVENT_GLOBAL_MAILBOX			(1 << 0)
#define EVENT_GLOBAL_Watchdog			(1 << 1)


// Events for Key:
#define EVENT_Key_TIMER_5MS_EVENT		(1 << 2)
#define EVENT_Key_TIMER_10MS_EVENT		(1 << 3)


// Events for led:
#define EVENT_Led_TIMER_5MS_EVENT		(1 << 2)
#define EVENT_Led_TIMER_20MS_EVENT		(1 << 3)


// Events for uart1:
#define EVENT_Uart1_TIMER_5MS_EVENT		(1 << 2)


// Events for taskComMPU:
#define EVENT_ComMPU_TIMER_10MS_EVENT            0x4
#define EVENT_ComMPU_SPI_IT_RX_FINISH_EVENT            0x8


// Events for taskPwrManage:
#define EVENT_PwrManage_TIMER_10MS_EVENT            0x4
#define EVENT_PwrManage_TIMER_10Min_TimeOut            0x8
#define EVENT_PwrManage_TIMER_5Min_TimeOut           0x10
#define EVENT_PwrManage_TIMER_3S_TimeOut           0x20
#define EVENT_PwrManage_TIMER_SOCInit_TimeOut           0x40
#define EVENT_PwrManage_TIMER_50Ms_TimeOut           0x80


#endif

