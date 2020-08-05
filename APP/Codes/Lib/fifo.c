/*
 * @Author: J
 * @Date: 2020-05-29 09:49:07
 * @LastEditTime: 2020-08-05 17:14:57
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \J0001-RTOS\APP\Codes\Lib\fifo.c
 */


/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include "fifo.h"

#if 0
#pragma inline(fifo_GetLen)
#pragma inline(fifo_GetFreeSpace)
#pragma inline(fifo_insert)
#pragma inline(fifo_retrieve)
#pragma inline(Fifo_strchr)
#endif

/************************************************************************
    Function :
    Description : Init the fifo structure.
            _fifo : reference to _fifo struct.
            size : size of the fifo.
            retval ERROR if memory allocation fails or OK.
    Change history:
    Note:
    Author:
************************************************************************/
u8 fifo_Init(fifo_TypeDef *_fifo, u8 *data, u32 size)
{
    /* check for a valid pointer */
    if (_fifo == NULL)
    {
        return ERROR;
    }

    /* if fifo size is null nothing to do */
    if (size == 0)
    {
        memset((u8 *)_fifo, 0x00, sizeof(fifo_TypeDef));
        return ERROR;
    }
    else
    {
        /* allocate fifo space. */
        _fifo->baseAddr = data;

        if (_fifo->baseAddr == NULL)
        {
            /* memory allocation failure. */
#ifdef SZ_LOG
            putLine();
            putString("----> Malloc Fail!");
#endif

            return ERROR;
        }
        else
        {
            memset((u8 *)_fifo->baseAddr, 0x00, sizeof(u8) * size);
            /* tail = head --> empty fifo */
            _fifo->head = 0;
            _fifo->tail = 0;
            _fifo->size = size;
            return SUCCESS;
        }
    }
}

/************************************************************************
    Function :
    Description : DeInit the fifo structure.
            _fifo : reference to _fifo struct.
    Change history:
    Note:
    Author:
************************************************************************/
u8 fifo_DeInit(fifo_TypeDef *_fifo)
{
    u8 status = ERROR;

    if (_fifo == NULL)
    {
        status = ERROR;
    }
    else
    {
        if (_fifo->baseAddr != NULL)
        {
            /* optional (clear memory region) */
            memset((u8 *)_fifo->baseAddr, 0x00, _fifo->size);
            memset((u8 *)_fifo, 0x00, sizeof(fifo_TypeDef));
            status = SUCCESS;
        }
        else
        {
            /* FiFo invalid base address. */
            status = ERROR;
        }
    }

    return status;
}

/************************************************************************
    Function :
    Description : Reset the fifo structure.
            _fifo : reference to _fifo struct.
    Change history:
    Note:
    Author:
************************************************************************/
u8 fifo_Reset(fifo_TypeDef *_fifo)
{
    if (_fifo == NULL)
    {
        return ERROR;
    }
    else
    {
        if (_fifo->baseAddr != NULL)
        {
            /* optional (clear memory region) */
            //memset((u8 *)_fifo->baseAddr, 0x00, _fifo->size);

            _fifo->head = 0;
            _fifo->tail = 0;
            return SUCCESS;
        }
        else
        {
            /* FiFo invalid base address. */
            return ERROR;
        }
    }
}

/************************************************************************
    Function :
    Description : returns how much data is stored in the fifo.
            _fifo : reference to _fifo struct.
    Change history:
    Note:
    Author:
************************************************************************/
u32 fifo_GetLen(fifo_TypeDef *_fifo)
{
    if (_fifo == NULL)
    {
        return 0;
    }
    else
    {
        return (_fifo->tail - _fifo->head);
    }
}

/************************************************************************
    Function :
    Description : returns how much free sapce in the fifo.
            _fifo : reference to _fifo struct.
    Change history:
    Note:
    Author:
************************************************************************/
u32 fifo_GetFreeSpace(fifo_TypeDef *_fifo)
{
    if (_fifo == NULL)
    {
        return 0;
    }
    else
    {
        return (_fifo->size - (_fifo->tail - _fifo->head));
    }
}

/************************************************************************
    Function :
    Description : returns SET if the FIFO is full.
            _fifo : reference to _fifo struct.
    Change history:
    Note:
    Author:
************************************************************************/
FlagStatus fifo_NotFull(fifo_TypeDef *_fifo)
{
    if (_fifo == NULL)
    {
        return RESET;
    }
    else
    {
        return ((_fifo->tail - _fifo->head) != _fifo->size) ? SET : RESET;
    }
}

/************************************************************************
    Function :
    Description : Insert data to the fifo.
            _fifo : reference to _fifo struct.
            data : reference to data buffer.
            len : data buffer length.
    Change history:
    Note:
    Author:
************************************************************************/
u32 fifo_insert(fifo_TypeDef *_fifo, u8 *data, u32 len)
{
    u32 real_len;
    u32 offset;
    u32 end;

    offset = _fifo->size - (_fifo->tail - _fifo->head);
    /*Actual size of data we can write*/
    real_len = MIN_VAL(len, offset);
    /*Offset of write pointer*/
    offset = _fifo->tail % _fifo->size;
    /*End position before buffer being warpped*/
    end = MIN_VAL(real_len, _fifo->size - offset);

    /*Copy data to buffer before warp */
    memcpy(_fifo->baseAddr + offset, data, end);

    /*Copy data to buffer after warp*/
    memcpy(_fifo->baseAddr, data + end, real_len - end);

    _fifo->tail += real_len;

    return real_len;
}

/************************************************************************
    Function :
    Description : Retrieve data from the fifo.
            _fifo : reference to _fifo struct.
            data : reference to data buffer.
            len : data buffer length.
    Change history:
    Note:
    Author:
************************************************************************/
u32 fifo_retrieve(fifo_TypeDef *_fifo, u8 *data, u32 len)
{
    u32 real_len;
    u32 offset;
    u32 end;

    /*获取插入fifo中数据的真实长度*/
    offset = _fifo->tail - _fifo->head;
    real_len = MIN_VAL(len, offset);

    //得到偏移量
    offset = _fifo->head % _fifo->size;

    end = MIN_VAL(real_len, _fifo->size - offset);

    memcpy(data, _fifo->baseAddr + offset, end);

    memcpy(data + end, _fifo->baseAddr, real_len - end);

    _fifo->head += real_len;

    return real_len;
}

/*****************************************************************************
* 函数名:  串口通道帧尾获取函数
* 函数功能: 获取当前帧帧尾所在地址
* 输入参数: _fifo - fifo结构体
* 输入参数: data-帧尾结束符
* 返回数据: fifo中接收到的数据到字节data第一次出现的长度
*****************************************************************************/
u32 Fifo_strchr(fifo_TypeDef *_fifo, u8 data)
{
    u32 remaining = 0;
    u32 len = 0;
    u32 head;

    head = _fifo->head;
    remaining = _fifo->tail - head;
    head = head % _fifo->size;

    while (remaining)
    {
        len++;

        if (data == *((_fifo->baseAddr + head)))
        {
            return len;
        }

        head++;
        remaining--;
        head %= _fifo->size;
    }

    return 0;
}
/*
init[20] = {1,1,2,2...}

data[2] = {3,3}
fifo_insert(xx,data,2)

u32 fifo_insert(fifo_TypeDef *_fifo, u8 *data, u32 len)
{
    u32 real_len;
    u32 offset;
    u32 end;

    offset = _fifo->size - (_fifo->tail - _fifo->head);
	//offset = 20 - (4-0) = 16
    //我们可以写入的数据的实际大小
    real_len = MIN_VAL(len, offset);
	//real_len = 2
    //写指针的偏移量
    offset = _fifo->tail % _fifo->size;
	//offset = 4%20 = 4
    //End position before buffer being warpped
    end = MIN_VAL(real_len, _fifo->size - offset);
	//end = 2,

   // Copy data to buffer before warp 
    memcpy(_fifo->baseAddr + offset, data, end);
	

    //Copy data to buffer after warp
    memcpy(_fifo->baseAddr, data + end, real_len - end);

    _fifo->tail += real_len;

    return real_len;
}
*/
