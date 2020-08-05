/*
 * @Author: your name
 * @Date: 2020-05-29 09:49:07
 * @LastEditTime: 2020-08-05 15:29:11
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \J0001-RTOS\APP\Codes\Driver\led.h
 */
#ifndef _LED_H
#define _LED_H
#include "sys.h"
#include "ResourceConfig.h"

u32 SendMsgToLed(u8 *MsgData, u16 MsgLen);
#endif
