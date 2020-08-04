/*
 * @Author: your name
 * @Date: 2020-06-05 09:58:51
 * @LastEditTime: 2020-08-03 11:19:53
 * @LastEditors: your name
 * @Description: In User Settings Edit
 * @FilePath: \J0001-RTOS\APP\Codes\ServiceLayer\MsgDefine.h
 */
#ifndef MSG_DEFINE_H
#define MSG_DEFINE_H

#include "sys.h"

typedef enum
{
    EVENT_VEHICLEIO_SEND_ACC_DATA = 0, /*4*/
    EVENT_SEND_DATA_TO_UART1,                        /*5*/
    EVENT_VEHICLEIO_SEND_ASTERN_DATA,                /*6*/
    EVENT_VEHICLEIO_SEND_AVN_DATA,                   /*7*/
    EVENT_VEHICLEIO_SEND_ILL_LEVEL_DATA,             /*8*/
    EVENT_KEY_STATUS,                                /*9*/

    EVENT_MAX_CMD,
} EnumSysMsgTb_t;

#endif
