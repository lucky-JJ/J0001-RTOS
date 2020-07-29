#ifndef _TMR_H
#define _TMR_H

#include "Std_Types.h"


extern struct tTMGR TMRMgr;
typedef void (*AlarmCallBackEntry)(void);

struct tTMR;

typedef struct tTMR * pTmr;
typedef uint8_t TMR_STATE;


#define INVALID_TMR_HANDLE \
		NULL

#define TMR_TIMEOUT_TRIGGERED \
		(uint32_t)(0)
#define TMR_TIMEOUT_CYCLE_SINGLE_SHOT \
		TMR_TIMEOUT_TRIGGERED

typedef struct
{
	TMR_STATE State;
	uint32_t DiffTimeOutVal;
	uint32_t CycleTimeOutVal;
	pTmr hPrevTmr;
	pTmr hNextTmr;
} TMR_RAM;

/* ALARM 配置信息表结构 */
typedef struct
{
    uint8_t                     taskId;
    uint32_t                    setEvent;
    AlarmCallBackEntry     callBack;

} TmrConfigTable_St;

typedef TmrConfigTable_St * pTmrCfgTbt;

typedef struct tTMR
{
	TMR_RAM *    pRAMData;
	const TmrConfigTable_St *   configTable;
} TMR;

/* ============================================================================================== */
/* Function prototypes																			  */
/* ============================================================================================== */
TMR_STATE TMR_GetState(const pTmr hTmr);
uint8_t TMR_IsAlive(const pTmr hTmr);
void TMR_ClearTriggered(const pTmr hTmr);

/* ============================================================================================== */
/* TMGR - timer manager																			  */
/* ============================================================================================== */
struct tTMGR;
typedef struct tTMGR * pTmgr;


#define INVALID_TMGR_HANDLE \
		NULL

#define TMGR_ERR_NO_ERROR \
		(uint8_t)(0)
#define TMGR_ERR_TMR_INACTIVE \
		(uint8_t)(1)
#define TMGR_ERR_TMR_TRIGGERED \
		(uint8_t)(2)
#define TMGR_ERR_TMR_ACTIVE \
		(uint8_t)(3)
#define TMGR_ERR_TMR_PAUSED \
		(uint8_t)(4)
#define TMGR_ERR_TMR_UNKNOWN_STATE \
		(uint8_t)(5)

typedef struct tTMGR
{
	pTmr hFirstTmr;
} TMGR;

void TMRG_Init(pTmr hTmr);
void TMGR_Shutdown(pTmgr this);
uint8_t TMGR_KillTimer(pTmgr this, pTmr hTmr);
uint8_t TMGR_StartTimer(pTmgr this, pTmr hTmr, uint32_t StartTimeOutVal, uint32_t CyclicTimeOutVal);
uint8_t TMGR_PauseTimer(pTmgr this, pTmr hTmr);
uint8_t TMGR_ResumeTimer(pTmgr this, pTmr hTmr);
uint8_t TMGR_RestartTimer(pTmgr this, pTmr hTmr, uint32_t StartTimeOutVal, uint32_t CyclicTimeOutVal);
uint8_t TMGR_ReSetTimer(pTmgr this, pTmr hTmr, uint32_t StartTimeOutVal, uint32_t CycleTimeOutVal);
uint8_t TMGR_QueryTimer(pTmgr this, pTmr hTmr, uint32_t * const pTimerVal);
void TMGR_HandleTick(pTmgr this);
uint32_t TMR_Get_DiffTimeOutVal(pTmr hTmr);

void TMGR_HandleTickCounter(pTmgr this, uint32_t SystickCounter);


#define TmgrInit(void)          TMRG_Init(&TMRMgr)
#define TmgrShutDown(void)      TMGR_Shutdown(&TMRMgr)
#define TmgrKillTimer(hTmr)     TMGR_KillTimer(&TMRMgr, hTmr) 
#define TmgrStartTimer(hTmr, StartTimeOutVal, CyclicTimeOutVal)   TMGR_StartTimer(&TMRMgr, hTmr, StartTimeOutVal, CyclicTimeOutVal)
#define TmgrPauseTimer(hTmr)    TMGR_PauseTimer(&TMRMgr, hTmr)
#define TmgrResumeTimer(hTmr)   TMGR_ResumeTimer(&TMRMgr, hTmr)
#define TmgrRestartTimer(hTmr, StartTimeOutVal, CyclicTimeOutVal) TMGR_RestartTimer(&TMRMgr, hTmr, StartTimeOutVal, CyclicTimeOutVal)
#define TmgrReSetTime(hTmr, StartTimeOutVal, CyclicTimeOutVal)    TMGR_ReSetTimer(&TMRMgr, hTmr, StartTimeOutVal, CyclicTimeOutVal)
#define TmgrQueryTimer(hTmr, pTimerVal)  TMGR_QueryTimer(&TMRMgr, hTmr, pTimerVal)
#define TmgrHandleTick(void)     TMGR_HandleTick(&TMRMgr)
#define TmgrGetDiffTimeOutVal(hTmr)  TMR_Get_DiffTimeOutVal(hTmr)
#define TmgrHandleTickCounter(SystickCounter) TMGR_HandleTickCounter(&TMRMgr, SystickCounter)

/* ============================================================================================== */
/* TMR object constructor																		  */
/* ============================================================================================== */
#define TMR_CONSTRUCT(ObjName, \
						P_CallbackFunc) \
		static TMR_RAM ObjName##RAM = \
		{ \
			0, \
			0, \
			0, \
			(pTmr)NULL, \
			(pTmr)NULL \
		}; \
		const struct tTMR ObjName = \
		{ \
			&ObjName##RAM, \
			P_CallbackFunc \
		}


#endif
