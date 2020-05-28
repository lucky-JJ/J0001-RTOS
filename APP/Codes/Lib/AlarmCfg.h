#ifndef _ALARM_CFG_H_
#define _ALARM_CFG_H_

#include "Tmr.h"
#if defined(USE_OS_FREERTOS)
#include <stdbool.h>
#endif


# define ALARM_NAME(name)             (ALARM_ID_##name)

#define CONFIG_ALARM_NUMBER   10

#define  ALARM_ID_BswService       (0)
#define  ALARM_ID_PwrManage       (1)
#define  ALARM_ID_PwrInitTimer       (2)
#define  ALARM_ID_PwrMgr10MinTimer       (3)
#define  ALARM_ID_PwrMgr5MinTimer       (4)
#define  ALARM_ID_PwrMgr3STimer       (5)
#define  ALARM_ID_PwrVoltageCheckTimer       (6)
#define  ALARM_ID_ComMPU       (7)
#define  ALARM_ID_Misc       (8)
#define  ALARM_ID_CAN       (9)

extern TMR AlarmTable[CONFIG_ALARM_NUMBER];

extern const bool AlarmAutoStartFlag[CONFIG_ALARM_NUMBER];

#endif
