#ifndef MSG_DEFINE_H
#define MSG_DEFINE_H

#include "sys.h"

typedef enum
{
    EVENT_VEHICLEIO_SEND_ACC_DATA = MSG_USER_DEFINE,	/*4*/
    EVENT_SEND_DATA_TO_UART1,                   		/*5*/
    EVENT_VEHICLEIO_SEND_ASTERN_DATA,                	/*6*/
    EVENT_VEHICLEIO_SEND_AVN_DATA,                   	/*7*/
    EVENT_VEHICLEIO_SEND_ILL_LEVEL_DATA,             	/*8*/
    EVENT_KEY_SWITCH_REQ,								/*9*/

    EVENT_MAX_CMD,
} EnumSysMsgTb_t;


#endif
