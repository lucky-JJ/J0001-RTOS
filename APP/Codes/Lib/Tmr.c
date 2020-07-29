#include "Tmr.h"
#include "Os.h"
#include "AlarmCfg.h"


#define TMR_STATE_INACTIVE \
    (uint8_t)(0)
#define TMR_STATE_TRIGGERED \
    (uint8_t)(1)
#define TMR_STATE_ACTIVE \
    (uint8_t)(2)
#define TMR_STATE_PAUSED \
    (uint8_t)(3)

struct tTMGR TMRMgr = {NULL};


/* ---------------------------------------------------------------------------------------------- */
TMR_STATE TMR_GetState(const pTmr hTmr)
/* ---------------------------------------------------------------------------------------------- */
{
	return hTmr->pRAMData->State;
}

/* ---------------------------------------------------------------------------------------------- */
uint8_t TMR_IsAlive(pTmr hTmr)
/* ---------------------------------------------------------------------------------------------- */
{
	if ((TMR_STATE_INACTIVE == hTmr->pRAMData->State) || (TMR_STATE_TRIGGERED == hTmr->pRAMData->State))
	{
		return FALSE;
	}
	else
	{
		/* Nothing to do */
	}

	return TRUE;
}

uint32_t TMR_Get_DiffTimeOutVal(pTmr hTmr)
{
	return hTmr->pRAMData->DiffTimeOutVal;
}

/* ---------------------------------------------------------------------------------------------- */
void TMR_ClearTriggered(pTmr hTmr)
/* ---------------------------------------------------------------------------------------------- */
{
	if (TMR_STATE_TRIGGERED == hTmr->pRAMData->State)
	{
		hTmr->pRAMData->State = TMR_STATE_INACTIVE;
	}
	else
	{
		/* Nothing to do */
	}
}

/* ============================================================================================== */
/* TMGR																							  */
/* ============================================================================================== */
/* ---------------------------------------------------------------------------------------------- */
void TMGR_Init(pTmgr this)
/* ---------------------------------------------------------------------------------------------- */
{
	this->hFirstTmr = INVALID_TMR_HANDLE;
}

/* ---------------------------------------------------------------------------------------------- */
void TMGR_Shutdown(pTmgr this)
/* ---------------------------------------------------------------------------------------------- */
{
	while (INVALID_TMR_HANDLE != this->hFirstTmr)
	{
		TMGR_KillTimer(this, this->hFirstTmr);
	}
}

/* ---------------------------------------------------------------------------------------------- */
uint8_t TMGR_KillTimer(pTmgr this, pTmr hTmr)
/* ---------------------------------------------------------------------------------------------- */
{
	uint8_t RetVal;

	switch (hTmr->pRAMData->State)
	{
		case TMR_STATE_INACTIVE:
		{
			RetVal = TMGR_ERR_TMR_INACTIVE;

			break;
		}
		case TMR_STATE_TRIGGERED:
		{
			hTmr->pRAMData->State = TMR_STATE_INACTIVE;
			RetVal = TMGR_ERR_TMR_TRIGGERED;

			break;
		}
		case TMR_STATE_ACTIVE:
		{
			if (INVALID_TMR_HANDLE != hTmr->pRAMData->hPrevTmr)
			{
				/* Killed timer is not at the head of list */
				hTmr->pRAMData->hPrevTmr->pRAMData->hNextTmr = hTmr->pRAMData->hNextTmr;
			}
			else
			{
				/* Killed timer is at the head of list */
				this->hFirstTmr = hTmr->pRAMData->hNextTmr;
			}
			if (INVALID_TMR_HANDLE != hTmr->pRAMData->hNextTmr)
			{
				/* Killed timer is not at the tail of list */
				hTmr->pRAMData->hNextTmr->pRAMData->hPrevTmr = hTmr->pRAMData->hPrevTmr;
				hTmr->pRAMData->hNextTmr->pRAMData->DiffTimeOutVal += hTmr->pRAMData->DiffTimeOutVal;
			}
			else
			{
				/* Nothing to do */
			}

			hTmr->pRAMData->State = TMR_STATE_INACTIVE;
			RetVal = TMGR_ERR_NO_ERROR;

			break;
		}
		case TMR_STATE_PAUSED:
		{
			hTmr->pRAMData->State = TMR_STATE_INACTIVE;
			RetVal = TMGR_ERR_NO_ERROR;

			break;
		}
		default:
		{
			RetVal = TMGR_ERR_TMR_UNKNOWN_STATE;

			break;
		}
	}

	return RetVal;
}

/* ---------------------------------------------------------------------------------------------- */
uint8_t TMGR_StartTimer(pTmgr this, pTmr hTmr, uint32_t StartTimeOutVal, uint32_t CycleTimeOutVal)
/* ---------------------------------------------------------------------------------------------- */
{
	/* Conditions:										*/
	/* StartTimeOutVal != TMR_TIMEOUT_TRIGGERED			*/
	/* StartTimeOutVal != TMR_TIMEOUT_CYCLE_SINGLE_SHOT	*/

	uint8_t RetVal;


    DisableTmrInterrupt();
	switch (hTmr->pRAMData->State)
	{
		case TMR_STATE_INACTIVE:
		case TMR_STATE_TRIGGERED:
		case TMR_STATE_PAUSED:
		{
			pTmr hRegisteredTmr;

			hRegisteredTmr = this->hFirstTmr;

			if (INVALID_TMR_HANDLE == hRegisteredTmr)
			{
				/* No timer in list */
				hTmr->pRAMData->hPrevTmr = INVALID_TMR_HANDLE;
				hTmr->pRAMData->hNextTmr = INVALID_TMR_HANDLE;
				this->hFirstTmr = hTmr;
			}
			else
			{
				/* Find the insertion point */
				while ((hRegisteredTmr->pRAMData->DiffTimeOutVal <= StartTimeOutVal) && (INVALID_TMR_HANDLE != hRegisteredTmr->pRAMData->hNextTmr))
				{
					StartTimeOutVal -= hRegisteredTmr->pRAMData->DiffTimeOutVal;
					hRegisteredTmr = hRegisteredTmr->pRAMData->hNextTmr;
				}

				if ((hRegisteredTmr->pRAMData->DiffTimeOutVal <= StartTimeOutVal) || (INVALID_TMR_HANDLE != hRegisteredTmr->pRAMData->hPrevTmr))
				{
					/* Insertion point != head of list */
					if (hRegisteredTmr->pRAMData->DiffTimeOutVal <= StartTimeOutVal)
					{
						/* Insertion point == tail of list */
						hTmr->pRAMData->hPrevTmr = hRegisteredTmr;
						hTmr->pRAMData->hNextTmr = INVALID_TMR_HANDLE;
						hRegisteredTmr->pRAMData->hNextTmr = hTmr;
						StartTimeOutVal -= hRegisteredTmr->pRAMData->DiffTimeOutVal;
					}
					else
					{
						/* Insertion point != tail of list */
						hTmr->pRAMData->hPrevTmr = hRegisteredTmr->pRAMData->hPrevTmr;
						hTmr->pRAMData->hNextTmr = hRegisteredTmr;
						hTmr->pRAMData->hPrevTmr->pRAMData->hNextTmr = hTmr;
						hRegisteredTmr->pRAMData->hPrevTmr = hTmr;
						hRegisteredTmr->pRAMData->DiffTimeOutVal -= StartTimeOutVal;
					}
				}
				else
				{
					/* Insertion point == head of list */
					hTmr->pRAMData->hPrevTmr = INVALID_TMR_HANDLE;
					hTmr->pRAMData->hNextTmr = hRegisteredTmr;
					hRegisteredTmr->pRAMData->hPrevTmr = hTmr;
					hRegisteredTmr->pRAMData->DiffTimeOutVal -= StartTimeOutVal;
					this->hFirstTmr = hTmr;
				}
			}
			hTmr->pRAMData->DiffTimeOutVal = StartTimeOutVal;
			hTmr->pRAMData->CycleTimeOutVal = CycleTimeOutVal;
			hTmr->pRAMData->State = TMR_STATE_ACTIVE;
			RetVal = TMGR_ERR_NO_ERROR;

			break;
		}
		case TMR_STATE_ACTIVE:
		{
			RetVal = TMGR_ERR_TMR_ACTIVE;

			break;
		}
		default:
		{
			RetVal = TMGR_ERR_TMR_UNKNOWN_STATE;

			break;
		}
	}
    EnableTmrInterrupt();
    
	return RetVal;
}

/* ---------------------------------------------------------------------------------------------- */
uint8_t TMGR_PauseTimer(pTmgr this, pTmr hTmr)
/* ---------------------------------------------------------------------------------------------- */
{
	uint8_t RetVal;


    DisableTmrInterrupt();
	switch (hTmr->pRAMData->State)
	{
		case TMR_STATE_INACTIVE:
		case TMR_STATE_TRIGGERED:
		{
			RetVal = TMGR_ERR_TMR_INACTIVE;

			break;
		}
		case TMR_STATE_ACTIVE:
		{
			pTmr hCurrTmr;
			uint32_t TimerVal;

			hCurrTmr = this->hFirstTmr;

			if (hTmr != hCurrTmr)
			{
				TimerVal = 0;
				do
				{
					TimerVal += hCurrTmr->pRAMData->DiffTimeOutVal;
					hCurrTmr = hCurrTmr->pRAMData->hNextTmr;
				} while ((hTmr != hCurrTmr) && (INVALID_TMR_HANDLE != hCurrTmr));

				if (INVALID_TMR_HANDLE != hCurrTmr)
				{
					TimerVal += hCurrTmr->pRAMData->DiffTimeOutVal;
				}
				else
				{
					/* Nothing to do */
				}
			}
			else
			{
				TimerVal = hCurrTmr->pRAMData->DiffTimeOutVal;
			}

			RetVal = TMGR_KillTimer(this, hTmr);
			if (TMGR_ERR_NO_ERROR == RetVal)
			{
				/* Save start delay in diff timer */
				hTmr->pRAMData->DiffTimeOutVal = TimerVal;
				hTmr->pRAMData->State = TMR_STATE_PAUSED;
			}
			else
			{
				/* Nothing to do */
			}

			break;
		}
		case TMR_STATE_PAUSED:
		{
			RetVal = TMGR_ERR_NO_ERROR;

			break;
		}
		default:
		{
			RetVal = TMGR_ERR_TMR_UNKNOWN_STATE;

			break;
		}
	}
    EnableTmrInterrupt();

	return RetVal;
}

/* ---------------------------------------------------------------------------------------------- */
uint8_t TMGR_ResumeTimer(pTmgr this, pTmr hTmr)
/* ---------------------------------------------------------------------------------------------- */
{
	uint8_t RetVal;


    DisableTmrInterrupt();
	switch (hTmr->pRAMData->State)
	{
		case TMR_STATE_INACTIVE:
		case TMR_STATE_TRIGGERED:
		{
			RetVal = TMGR_ERR_TMR_INACTIVE;

			break;
		}
		case TMR_STATE_ACTIVE:
		{
			RetVal = TMGR_ERR_NO_ERROR;

			break;
		}
		case TMR_STATE_PAUSED:
		{
			RetVal = TMGR_StartTimer(this, hTmr, hTmr->pRAMData->DiffTimeOutVal, hTmr->pRAMData->CycleTimeOutVal);

			break;
		}
		default:
		{
			RetVal = TMGR_ERR_TMR_UNKNOWN_STATE;

			break;
		}
	}
    EnableTmrInterrupt();
    
	return RetVal;
}

/* ---------------------------------------------------------------------------------------------- */
uint8_t TMGR_RestartTimer(pTmgr this, pTmr hTmr, uint32_t StartTimeOutVal, uint32_t CycleTimeOutVal)
/* ---------------------------------------------------------------------------------------------- */
{
    uint8_t RetVal;


    DisableTmrInterrupt();
    TMGR_KillTimer(this, hTmr);

    RetVal = TMGR_StartTimer(this, hTmr, StartTimeOutVal, CycleTimeOutVal);
    EnableTmrInterrupt();

    return RetVal;
}

/* ---------------------------------------------------------------------------------------------- */
uint8_t TMGR_ReSetTimer(pTmgr this, pTmr hTmr, uint32_t StartTimeOutVal, uint32_t CycleTimeOutVal)
/* ---------------------------------------------------------------------------------------------- */
{
	uint8_t RetVal;
	TMR_STATE TimerState;


    DisableTmrInterrupt();
	TimerState = hTmr->pRAMData->State;
	RetVal = TMGR_KillTimer(this, hTmr);

	if (TMGR_ERR_NO_ERROR == RetVal)
	{
		if (TMR_STATE_PAUSED == TimerState)
		{
			/* Save start delay in diff timer */
			hTmr->pRAMData->DiffTimeOutVal = StartTimeOutVal;
			hTmr->pRAMData->CycleTimeOutVal = CycleTimeOutVal;
			hTmr->pRAMData->State = TMR_STATE_PAUSED;
		}
		else
		{
			RetVal = TMGR_StartTimer(this, hTmr, StartTimeOutVal, CycleTimeOutVal);
		}
	}
	else
	{
		/* Nothing to do */
	}
    EnableTmrInterrupt();

	return RetVal;
}

/* ---------------------------------------------------------------------------------------------- */
uint8_t TMGR_QueryTimer(pTmgr this, pTmr hTmr, uint32_t * const pTimerVal)
/* ---------------------------------------------------------------------------------------------- */
{
	uint8_t RetVal;

	switch (hTmr->pRAMData->State)
	{
		case TMR_STATE_INACTIVE:
		{
			RetVal = TMGR_ERR_TMR_INACTIVE;

			break;
		}
		case TMR_STATE_TRIGGERED:
		{
			*pTimerVal = TMR_TIMEOUT_TRIGGERED;
			RetVal = TMGR_ERR_TMR_TRIGGERED;

			break;
		}
		case TMR_STATE_ACTIVE:
		{
			pTmr hCurrTmr;
			uint32_t TimerVal;

			hCurrTmr = this->hFirstTmr;

			if (hTmr != hCurrTmr)
			{
				TimerVal = 0;
				do
				{
					TimerVal += hCurrTmr->pRAMData->DiffTimeOutVal;
					hCurrTmr = hCurrTmr->pRAMData->hNextTmr;
				} while ((hTmr != hCurrTmr) && (INVALID_TMR_HANDLE != hCurrTmr));

				if (INVALID_TMR_HANDLE != hCurrTmr)
				{
					TimerVal += hCurrTmr->pRAMData->DiffTimeOutVal;
				}
				else
				{
					/* Nothing to do */
				}
			}
			else
			{
				TimerVal = hCurrTmr->pRAMData->DiffTimeOutVal;
			}

			*pTimerVal = TimerVal;
			RetVal = TMGR_ERR_NO_ERROR;

			break;
		}
		case TMR_STATE_PAUSED:
		{
			RetVal = TMGR_ERR_TMR_PAUSED;

			break;
		}
		default:
		{
			RetVal = TMGR_ERR_TMR_UNKNOWN_STATE;

			break;
		}
	}

	return RetVal;
}

/* ---------------------------------------------------------------------------------------------- */
void TMGR_HandleTick(pTmgr this)
/* ---------------------------------------------------------------------------------------------- */
{
	uint32_t TimerVal;
	pTmr hTmr;

	hTmr = this->hFirstTmr;

	if (INVALID_TMR_HANDLE != hTmr)
	{
		--(hTmr->pRAMData->DiffTimeOutVal);
		while ((INVALID_TMR_HANDLE != hTmr) && (TMR_TIMEOUT_TRIGGERED == hTmr->pRAMData->DiffTimeOutVal))
		{
			TimerVal = hTmr->pRAMData->CycleTimeOutVal;
			if(TMGR_ERR_TMR_UNKNOWN_STATE != TMGR_KillTimer(this, hTmr))
			{
				if (TMR_TIMEOUT_CYCLE_SINGLE_SHOT == TimerVal)
				{
					/* Single-shot timer */
					hTmr->pRAMData->State = TMR_STATE_TRIGGERED;
				}
				else
				{
					/* Cyclic timer */
					TMGR_StartTimer(this, hTmr, TimerVal, TimerVal);
				}
                
				if(hTmr->configTable->setEvent != 0 && hTmr->configTable->taskId != 0)
                {
                    EventSendTaskFormISR(hTmr->configTable->taskId, hTmr->configTable->setEvent);
                }
                else
                {
                    if(hTmr->configTable->callBack != NULL)
                    {
                        hTmr->configTable->callBack();
                    }
                }
				hTmr = this->hFirstTmr;
			}
			else
			{
				 //NVIC_SystemReset();
			}
		}
	}
	else
	{
		/* Nothing to do */
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void TMGR_HandleTickCounter(pTmgr this,uint32_t SystickCounter)
////////////////////////////////////////////////////////////////////////////////////////////////////
{
	uint32_t TimerVal = (uint32_t)0;
	pTmr hTmr = this->hFirstTmr;
	uint16_t index,LoopCnt;
	

#if 0
	if (INVALID_TMR_HANDLE != hTmr)
	{
		--(hTmr->pRAMData->DiffTimeOutVal);
		while ((INVALID_TMR_HANDLE != hTmr) && (TMR_TIMEOUT_TRIGGERED == hTmr->pRAMData->DiffTimeOutVal))
		{
			TimerVal = hTmr->pRAMData->CycleTimeOutVal;
			if(TMGR_ERR_TMR_UNKNOWN_STATE != TMGR_KillTimer(this, hTmr))
			{
				if (TMR_TIMEOUT_CYCLE_SINGLE_SHOT == TimerVal)
				{
					// Single-shot timer
					hTmr->pRAMData->State = TMR_STATE_TRIGGERED;
				}
				else
				{
					// Cyclic timer
					TMGR_StartTimer(this, hTmr, TimerVal, TimerVal);
				}
				
				if((TimerVal > 0) && (SystickCounter > TimerVal))
				{
					LoopCnt = (SystickCounter / TimerVal);
				}
				else
				{
					LoopCnt = 1;
				}
				
				for(index = 0;index<LoopCnt;index++)
				{
						if(hTmr->configTable->setEvent != 0 && hTmr->configTable->taskId != 0)
		                {
		                    OSIF_EVSendTaskFormISR(hTmr->configTable.taskId, hTmr->configTable->setEvent);
		                }
		                else
		                {
		                    if(hTmr->configTable.callBack != NULL)
		                    {
		                        hTmr->configTable->callBack();
		                    }
		                }
				}
				hTmr = this->hFirstTmr;
			}
			else
			{
				 //NVIC_SystemReset();
			}
		}
	}
	
	if(INVALID_TMR_HANDLE != hTmr && --SystickCounter >0)
	{
		while ((INVALID_TMR_HANDLE != hTmr) && (SystickCounter >= hTmr->pRAMData->DiffTimeOutVal))
		{
			hTmr = hTmr->pRAMData->hNextTmr;
		}
		if(INVALID_TMR_HANDLE != hTmr)
		{
			if(hTmr->pRAMData->DiffTimeOutVal > SystickCounter)
			{
 				hTmr->pRAMData->DiffTimeOutVal  -= SystickCounter;
			}
			else
			{
				hTmr->pRAMData->DiffTimeOutVal = 1;
			}
		}
	}
#endif

    if (INVALID_TMR_HANDLE != hTmr)
    {
        if(SystickCounter == 0)
        {
            SystickCounter = 1; 
        }
        
        while(SystickCounter)
        {
            if(hTmr->pRAMData->DiffTimeOutVal >= SystickCounter)
            {
                hTmr->pRAMData->DiffTimeOutVal -= SystickCounter;
                SystickCounter = 0;
            }
            else
            {
                SystickCounter = SystickCounter - hTmr->pRAMData->DiffTimeOutVal;
                hTmr->pRAMData->DiffTimeOutVal = 0;
            }
        
            while ((INVALID_TMR_HANDLE != hTmr) && (TMR_TIMEOUT_TRIGGERED == hTmr->pRAMData->DiffTimeOutVal))
            {
                TimerVal = hTmr->pRAMData->CycleTimeOutVal;
                if(TMGR_ERR_TMR_UNKNOWN_STATE != TMGR_KillTimer(this, hTmr))
                {
                    if (TMR_TIMEOUT_CYCLE_SINGLE_SHOT == TimerVal)
                    {
                        // Single-shot timer
                        hTmr->pRAMData->State = TMR_STATE_TRIGGERED;
                    }
                    else
                    {
                        // Cyclic timer
                        TMGR_StartTimer(this, hTmr, TimerVal, TimerVal);
                    }
                    
                    if((TimerVal > 0) && (SystickCounter > TimerVal))
                    {
                        LoopCnt = (SystickCounter / TimerVal);
                    }
                    else
                    {
                        LoopCnt = 1;
                    }
                    
                    for(index = 0; index< LoopCnt; index++)
                    {
						if(hTmr->configTable->setEvent != 0 && hTmr->configTable->taskId != 0)
		                {
		                    EventSendTaskFormISR(hTmr->configTable->taskId, hTmr->configTable->setEvent);
		                }
		                else
		                {
		                    if(hTmr->configTable->callBack != NULL)
		                    {
		                        hTmr->configTable->callBack();
		                    }
		                }
                    }
					
                    hTmr = this->hFirstTmr;
                }
                else
                {
                     //NVIC_SystemReset();
                }
            }
        }
    }
}

