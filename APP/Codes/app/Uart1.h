/*
 * @Author: your name
 * @Date: 2020-05-29 09:49:07
 * @LastEditTime: 2020-08-05 15:30:14
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \J0001-RTOS\APP\Codes\app\Uart1.h
 */

#include "sys.h"
#include "ResourceConfig.h"

typedef struct
{
    Message_t Msg;
    u8 Reserve;
    u16 Length;
} UART1_DataEvt_t;

typedef struct
{
    UART1_DataEvt_t Head;
    u8 Data[1];
} UART1_DataEvtSturt;

s32 SendMsgToUart1(u8 *MsgBuff, u16 MsgLen);