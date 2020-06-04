#ifndef _USART_H
#define _USART_H
#include "sys.h"
#include "stdio.h"	


extern UART_HandleTypeDef UART1_Handler; //UART句柄

void uart1_init(void *Uart1_TxHandle, void *Uart1_RxHandle);

void Uart1_TxIntHandle(void);
void Uart1_RxIntHandle(u8 byte);


void USART1_SendDataManage(void);

void USART1_ManageProc(u16 Tick);



#endif
