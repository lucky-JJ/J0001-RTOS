/*
 * @Author: J
 * @Date: 2020-07-28 09:50:44
 * @LastEditTime: 2020-07-29 09:50:58
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \APP\Codes\ServiceLayer\Os_Init.c
 */
#include "Os.h"
//#include "ResourceConfig.h"
//#include "SEGGER_RTT.h"
//#include "SEGGER_SYSVIEW.h"

//extern const SEGGER_SYSVIEW_OS_API SYSVIEW_X_OS_TraceAPI;

/********************************************************************* 
*
*       _cbSendSystemDesc()
*
*  Function description
*    Sends SystemView description strings.
*/
//static void _cbSendSystemDesc(void)
//{
//    SEGGER_SYSVIEW_SendSysDesc(SYSVIEW_SYSDESC_SYS_INFO);
//   SEGGER_SYSVIEW_SendSysDesc(SYSVIEW_SYSDESC_SYSTICK);
//}

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/
//void SEGGER_SYSVIEW_Conf(void)
//{
//    SEGGER_SYSVIEW_Init(SYSVIEW_TIMESTAMP_FREQ, SYSVIEW_CPU_FREQ, &SYSVIEW_X_OS_TraceAPI, _cbSendSystemDesc);
//    SEGGER_SYSVIEW_SetRAMBase(SYSVIEW_RAM_BASE);
//}

void Os_IsrInit(void)
{
    Irq_Init();
}
