#ifndef _TSK_CFG_
#define _TSK_CFG_

/******************************************************************
Task define
Para I------->name
Para II------>priority
Para III----->StackDepth
******************************************************************/
#ifdef STDTASKDEF
STDTASKDEF(Uart1,  	 1, 128)
STDTASKDEF(Key,  	 2, 50)
STDTASKDEF(Led,  	 3, 50)
#endif

#ifdef APPMODE
APPDEF(DEFAULTAPPMODE)
#endif

/******************************************************************
Semaphore define
Para I------->name
Para II------>init value
Para III------>max value
******************************************************************/
#ifdef SEMPHRDEF
SEMDEF(AuidoSem, 0, 1)
SEMDEF(I2c0Sem, 0, 1)
#endif

#ifdef MUTEXDEF
MUTEXDEF(RtcMtx)
MUTEXDEF(NVM)
MUTEXDEF(DBGMTX)
#endif

/******************************************************************
alarm define
Para I------->alarm name
Para II------>defalut self-start flag
Para III----->first start
Para IV------>
Para V-------> 1：active_task  2 callback
******************************************************************/
#ifdef ALARMDEF
//ALARMDEF(BswService, true, 0, 10,  1)

#endif

/******************************************************************
event define
Para I------->task name
Para II------>event name
Para III----->event mask
Para IV------>0:null  1：alarm active
******************************************************************/
#ifdef EVENTDEF

/* Global */
EVENTDEF(GLOBAL, MAILBOX, AUTO, 0)
EVENTDEF(GLOBAL, Watchdog, AUTO, 0)


#endif

#endif   // _tskHandl_cf

