/*
 * @Author: J
 * @Date: 2020-05-29 09:49:07
 * @LastEditTime: 2020-08-05 16:56:32
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \J0001-RTOS\APP\Codes\Lib\fifo.h
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _FIFO_H_
#define _FIFO_H_
/* Includes ------------------------------------------------------------------*/

#include "sys.h"


typedef struct
{
    u32 head;  /* output conut */
    u32 tail;     /*  input count   */
    u32 size;   /*  total size  */
    u8 *baseAddr;
} fifo_TypeDef;

u8 fifo_Init(fifo_TypeDef *_fifo, u8 *data, u32 size);
u8 fifo_DeInit(fifo_TypeDef *_fifo);
u8 fifo_Reset(fifo_TypeDef *_fifo);


u32 fifo_insert(fifo_TypeDef *_fifo, u8 *data, u32 len);
u32 fifo_retrieve(fifo_TypeDef *_fifo, u8 *data, u32 len);

u32 fifo_GetLen(fifo_TypeDef *_fifo);
FlagStatus fifo_NotFull(fifo_TypeDef *_fifo);
u32 fifo_GetFreeSpace(fifo_TypeDef *_fifo);
u32  Fifo_strchr(fifo_TypeDef *_fifo, u8 data);

#endif

