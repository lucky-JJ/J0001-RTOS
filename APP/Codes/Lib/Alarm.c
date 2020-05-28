#include "OSIF.h"
#include "Alarm.h"
#include "AlarmCfg.h"
// ALARM 管理提供给外部的公共调用函数

 
#if CONFIG_ALARM_NUMBER > 0
// ALARM 控制块表声明
extern TMR AlarmTable[CONFIG_ALARM_NUMBER];
extern const bool AlarmAutoStartFlag[CONFIG_ALARM_NUMBER];
#endif

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
        OS_HOOK_ErrorHook(E_OS_VALUE,OSServiceId_SetRelAlarm, almId);
        RetVal = 1;
    }
    else
    {
        // 如果起始时间值大于counter配置的最大值，返回E_OS_VALUE
        if( increment > MAX_ALLOWED_TICK_VALUE)
        {
            OS_HOOK_ErrorHook(E_OS_VALUE, OSServiceId_SetRelAlarm, almId);
            RetVal = 2;
        }
        else
        {
            // 对于周期性ALARM，如果周期值大于counter配置的最大值或低于最小周期值，返回E_OS_VALUE
            if(( cycle != 0 ) && ( (cycle > MAX_ALLOWED_TICK_VALUE) || (cycle < MIN_ALLOWED_CYCLE_TICK_VALUE)))
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

// 内部函数
void Alarm_Init(void)
{
#undef _TSK_CFG_
#undef ALARMDEF
#define ALARMDEF(AlarmName, Flag, Cyctim, Cycphs, ActiveMethod)  \
    if(Flag == true)                               \
    {                                              \
        SetAlarm(ALARM_ID_##AlarmName, Cyctim, Cycphs);\
    }
#include "tsk.cfg"
#undef ALARMDEF
}
 