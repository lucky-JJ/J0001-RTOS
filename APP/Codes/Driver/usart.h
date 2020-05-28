#ifndef _USART_H
#define _USART_H
#include "sys.h"
#include "stdio.h"	
//////////////////////////////////////////////////////////////////////////////////	 
//********************************************************************************
//V1.0修改说明 
////////////////////////////////////////////////////////////////////////////////// 	
#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	
extern UART_HandleTypeDef UART1_Handler; //UART句柄



void uart1_init(void *Uart1_TxHandle, void *Uart1_RxHandle);

void Uart1_TxIntHandle(void);


void Uart1_RxIntHandle(u8 byte);

u8 USART1_SendData(u8 *pData, u16 len);



#endif
