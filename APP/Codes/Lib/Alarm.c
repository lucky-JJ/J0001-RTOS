#include "Os.h"
#include "Alarm.h"
#include "AlarmCfg.h"
// ALARM 管理提供给外部的公共调用函数

 
#if CONFIG_ALARM_NUMBER > 0
// ALARM 控制块表声明
extern const AlarmAutoStartParam_t AlarmAutoStartCfg[CONFIG_ALARM_NUMBER];

/**
 * @brief
 *    SetRelAlarm：设置相对时间ALARM。
 * @param[in]almId: 指向ALARM控制块的指针。
 * @param[in]   increment: 以tick为单位的相对起始时间值。
 * @param[in]   cycle: 周期性ALARM的周期值。对于一次性的ALARM，该值必须为0。
 * @returns     E_OK：成功。
 * @returns     E_OS_STATE:指定的ALARM已经在用。
 * @returns     [E_OS_ID:无效的ALARM。]
 * @returns     [E_OS_VALUE:输入的起始时间和周期参数超出配置的范围。]
 */
StatusType SetAlarm(AlarmType almId, TickType increment, TickType cycle)
{ 
    StatusType RetVal = 0;

    
    if(almId >= CONFIG_ALARM_NUMBER)
    {
        OS_HOOK_ErrorHook(E_OS_VALUE, OSServiceId_SetRelAlarm, almId);
        RetVal = 1;
    }
    else
    {
        // 对于周期性ALARM，如果周期值大于counter配置的最大值或低于最小周期值，返回E_OS_VALUE
        if(( cycle != 0 ) && (cycle < MIN_ALLOWED_CYCLE_TICK_VALUE))
        {
            OS_HOOK_ErrorHook(E_OS_VALUE, OSServiceId_SetRelAlarm, almId);
            RetVal = 3;
        }
        else
        {
            // 如果指定ALARM已经在用，返回E_OS_STATE
            if(AlarmTable[almId].pRAMData->State != (AlarmType)0 )
            {
                OS_HOOK_ErrorHook(E_OS_STATE, OSServiceId_SetRelAlarm, almId);
                RetVal = 4;
            }
            else
            {
                // 屏蔽中断
                DisableOSInt();
                // 调用SetAlarm设置ALARM
                TmgrStartTimer(&AlarmTable[almId], increment, cycle);
                // 打开中断
                EnableOSInt();
            }
        }
    }

    return RetVal;
}

/**
 * @brief
 *    CancelAlarm：取消设置的ALARM。
 * @param[in]almId: 指向ALARM控制块的指针。
 * @returns     E_OK：成功。
 * @returns     E_OS_NOFUNC:指定的ALARM未在使用。
 * @returns     [E_OS_ID:无效的输入参数。]
 */
StatusType CancelAlarm( AlarmType almId)
{
    StatusType RetVal = 0;

    
    if(almId >= CONFIG_ALARM_NUMBER)
    {
        OS_HOOK_ErrorHook(E_OS_VALUE,OSServiceId_CancelAlarm, almId);
        RetVal = 1;
    }
    
    // 屏蔽中断
    DisableOSInt();
    RetVal = TmgrKillTimer(&AlarmTable[almId]);
    // 打开中断
    EnableOSInt();
    
    return RetVal;
}

void Alarm_Counter(TickType SystickCounter)
{
    TmgrHandleTickCounter(SystickCounter);
}

FUNC(StatusType, OS_CODE) GetCounterValue (
                                            VAR(CounterType, AUTOMATIC) CounterID,
                                            VAR(TickRefType, AUTOMATIC) Value)
{
  VAR(StatusType, AUTOMATIC)  ev;


    /* [SWS_Os_00381] If the input parameter <CounterID> in a call of
    GetElapsedValue() is not valid GetElapsedValue() shall return E_OS_ID. */
    if (CounterID >= OS_COUNTER_COUNT) 
    {
        ev = E_ID;
    } 
    else if (Value == NULL) 
    {
        ev = E_PARAM_POINTER;
    } 
    else
    {
        CONST(TickType, AUTOMATIC) local_curr_value = Os_GetMilliseconds();

        (*Value) = local_curr_value;
        ev = E_OK;
    }
    return ev;
}

FUNC(StatusType, OS_CODE) GetElapsedValue (
                                            VAR(CounterType, AUTOMATIC) CounterID,
                                            VAR(TickRefType, AUTOMATIC) Value,
                                            VAR(TickRefType, AUTOMATIC) ElapsedValue)
{
  VAR(StatusType, AUTOMATIC)  ev;


    /* [SWS_Os_00381] If the input parameter <CounterID> in a call of
    GetElapsedValue() is not valid GetElapsedValue() shall return E_OS_ID. */
    if (CounterID >= OS_COUNTER_COUNT) 
    {
        ev = E_ID;
    } 
    else if ((Value == NULL) || (ElapsedValue == NULL)) 
    {
        ev = E_PARAM_POINTER;
    } 
    else
    {
        CONST(TickType, AUTOMATIC) local_value = (*Value);

        /* [SWS_Os_00382] If the input parameters in a call of GetElapsedValue()
            are valid, GetElapsedValue() shall return the number of elapsed ticks
            since the given <Value> value via <ElapsedValue> and shall return
            E_OK. (SRS_Frt_00034) */
        CONST(TickType, AUTOMATIC) local_curr_value = Os_GetMilliseconds();

/* [SWS_Os_00533] Caveats of GetElapsedValue(): If the timer already passed the
    <Value> value a second (or multiple) time, the result returned is wrong.
    The reason is that the service can not detect such a relative overflow. */
/* EG  TODO: Add support for HARDWARE counters */
      (*ElapsedValue) = (local_curr_value >= local_value)?
        /* Timer did not pass the <value> yet */
        (local_curr_value - local_value):
        /* Timer already passed the <value> */
        ((OsCounterMaxAllowedValue - (local_value - local_curr_value)) + 1U);

    /* [SWS_Os_00460] GetElapsedValue() shall return the current tick value of the
        counter in the <Value> parameter. */
        (*Value) = local_curr_value;

        ev = E_OK;
    }
    return ev;
}

// 内部函数
void Alarm_Init(void)
{
    uint8_t AlarmIndex;


    for(AlarmIndex = 0; AlarmIndex < CONFIG_ALARM_NUMBER; AlarmIndex++)
    {
        if(AlarmAutoStartCfg[AlarmIndex].AutoStartFlag == true)
        {
            SetAlarm(AlarmAutoStartCfg[AlarmIndex].Alarm_Id, 
                     AlarmAutoStartCfg[AlarmIndex].DiffTimeOutVal, 
                     AlarmAutoStartCfg[AlarmIndex].CycleTimeOutVal);
        }
    }
}
 
#endif