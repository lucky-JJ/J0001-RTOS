#ifndef _ALARM_H_
#define _ALARM_H_

#ifndef STATUSTYPE_DEFINED
#define STATUSTYPE_DEFINED
typedef unsigned char      StatusType;
#endif

/**
 * $Log: Alarm.h,v $
 * Revision 1.0  2008/03/29 12:23:00
 * 创建文件
*/
 
/**
 * @file osekAlarm.h
 * @brief
 *<li>功能：定义报警器相关的数据结构、宏，以及相关函数的申明。</li>
 * @author
 * @date 2008-03-29
 * <p>部门：
 */
 
/**
 * \defgroup  Alarm 报警
 * Alarm管理模块,主要实现报警的管理
 */
/**@{
*/
/** @name Alarm管理模块*/
//@{
 

#include "Std_Types.h"
/**************************** 引用部分 *********************************/
#include "Tmr.h"


#ifdef __cplusplus
extern "C" {
#endif

#if 1
typedef uint8_t      AlarmType;
typedef uint32_t     TickType;
typedef TMR     *    AlarmBaseRefType;
typedef uint32_t   * TickRefType;
typedef uint32_t     CounterType;

#define MAX_ALLOWED_TICK_VALUE            (0xffffffff)
#define MIN_ALLOWED_CYCLE_TICK_VALUE      (0x0001)

#define OSServiceId_SetRelAlarm    0x04
#define OSServiceId_CancelAlarm    0x05


#define ALARMCALLBACK(AlarmName)   void Alarm_##AlarmName##_CallBack(void)
#define EXTERNALARMCALLBACK(AlarmName)  void Alarm_##AlarmName##_CallBack(void)

/**************************** 声明部分 *********************************/
 
// ALARM 管理提供给外部的公共调用函数
 
/**
 * @brief
 *    SetAlarm：设置相对时间ALARM。
 * @param[in]almId: 指向ALARM控制块的指针。
 * @param[in]   increment: 以tick为单位的相对起始时间值。
 * @param[in]   cycle: 周期性ALARM的周期值。对于一次性的ALARM，该值必须为0。
 * @returns     E_OK：成功。
 * @returns     E_OS_STATE:指定的ALARM已经在用。
 * @returns     [E_OS_ID:无效的ALARM。]
 * @returns     [E_OS_VALUE:输入的起始时间和周期参数超出配置的范围。]
 */
StatusType SetAlarm(AlarmType almId, TickType increment, TickType cycle);

/**
 * @brief
 *    CancelAlarm：取消设置的ALARM。
 * @param[in]almId: 指向ALARM控制块的指针。
 * @returns     E_OK：成功。
 * @returns     E_OS_NOFUNC:指定的ALARM未在使用。
 * @returns     [E_OS_ID:无效的输入参数。]
 */
StatusType CancelAlarm( AlarmType almId );

void Alarm_Counter(TickType SystickCounter);

void Alarm_Init( void );

FUNC(StatusType, OS_CODE) GetCounterValue (
                                            VAR(CounterType, AUTOMATIC) CounterID,
                                            VAR(TickRefType, AUTOMATIC) Value);

FUNC(StatusType, OS_CODE) GetElapsedValue (
                                            VAR(CounterType, AUTOMATIC) CounterID,
                                            VAR(TickRefType, AUTOMATIC) Value,
                                            VAR(TickRefType, AUTOMATIC) ElapsedValue);
 
#if CONFIG_ALARM_NUMBER > 0
extern TMR AlarmTable[CONFIG_ALARM_NUMBER];
#endif
 
 
#define DeclareAlarm(alarm)

#endif

#ifdef __cplusplus
}
#endif
 
#endif
 
/* end of include file */
//@
/** @
}*/

