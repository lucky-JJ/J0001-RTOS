/*
 * @Author: your name
 * @Date: 2020-05-29 09:49:07
 * @LastEditTime: 2020-08-03 16:33:57
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \J0001-RTOS\APP\Codes\Driver\key.h
 */
#ifndef _KEY_H
#define _KEY_H
#include "sys.h"
#include "GPIO.h"
#include "Os.h"

#define Key0 GPIO_Read(IO_KEY0)
#define Key1 GPIO_Read(IO_KEY1)
#define Key2 GPIO_Read(IO_KEY2)
#define Key_WK_UP GPIO_Read(IO_WK_UP)

typedef struct
{
    Message_t Msg;
    u8 Key0_status : 2;
    u8 Key1_status : 2;
    u8 Key2_status : 2;
    u8 Key_WK_UP_status : 2;
} KeyData_t;

u32 SendMsgToKey(u8 *MsgData, u16 MsgLen);
#endif
