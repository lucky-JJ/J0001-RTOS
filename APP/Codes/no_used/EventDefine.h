#ifndef EVENT_DEFINE_H
#define EVENT_DEFINE_H


/*最多支持32个事件*/

// Events for all tasks:
#define EVENT_GLOBAL_MAILBOX			(1 << 0)
#define EVENT_GLOBAL_Watchdog			(1 << 1)
#define EVENT_TIMER_5MS					(1 << 2)
#define EVENT_TIMER_10MS				(1 << 3)
#define EVENT_TIMER_30MS				(1 << 4)

// Events for uart1:
#define EVENT_Uart1_TX_FINISH			(1 << 5)


// Events for taskComMPU:

#define EVENT_ComMPU_SPI_IT_RX_FINISH_EVENT            0x8








#endif

