#include "usart.h"
#include "fifo.h"
#include "Os.h"

static fifo_TypeDef Uart1_RxData_fifo;

u8 Uart1_RxDataBuffer[512];
u8 Uart1_CurrentRxBuf[256];

UART_HandleTypeDef UART1_Handler;

void Uart1_TxIntHandle(void)
{
    //Os_EventSendTask(TASK_NAME(Uart1), EVENT_Uart1_TX_FINISH);
}

void Uart1_RxIntHandle(u8 byte)
{
    fifo_insert(&Uart1_RxData_fifo, &byte, 1);
}

void uart1_init(void *Uart1_TxHandle, void *Uart1_RxHandle)
{
    /* 1.串口参数配置 */
    UART1_Handler.Instance = USART1;                    //USART1
    UART1_Handler.Init.BaudRate = 115200;               //波特率
    UART1_Handler.Init.WordLength = UART_WORDLENGTH_8B; //字长为8位数据格式
    UART1_Handler.Init.StopBits = UART_STOPBITS_1;      //一个停止位
    UART1_Handler.Init.Parity = UART_PARITY_NONE;       //无奇偶校验位
    UART1_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE; //无硬件流控
    UART1_Handler.Init.Mode = UART_MODE_TX_RX;          //收发模式
    UART1_Handler.UartRxCpl = (HAL_Callback_Func1)Uart1_RxHandle;
    UART1_Handler.UartTxCpl = (HAL_Callback_Func)Uart1_TxHandle;
    HAL_UART_Init(&UART1_Handler); //HAL_UART_Init()会使能UART1

    HAL_UART_Receive_IT(&UART1_Handler); //使能接收中断和错误中断：UART_IT_RXNE

    fifo_Init(&Uart1_RxData_fifo, &Uart1_RxDataBuffer[0], 512);
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
}

static u8 USART1_SendData(u8 *pData, u16 len)
{
    /*该函数把数据填充到发送数据缓冲区之后,会开启UART_IT_TXE,会触发发送中断,然后进入 UART_Transmit_IT 进行处理*/
    return HAL_UART_Transmit_IT(&UART1_Handler, pData, len);
}

void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&UART1_Handler);
}

void USART1_RxDataHandle(u8 *pData, u16 Len)
{
}

/*周期调用接收 , 通过此函数将数据解析,然后发送到需要处理的任务模块邮箱*/
static void USART1_RxDataParse(void)
{
    u16 RxLength = 0;

    if (0 == fifo_GetFreeSpace(&Uart1_RxData_fifo))
    {
        fifo_Init(&Uart1_RxData_fifo, Uart1_RxDataBuffer, 512);
    }

    RxLength = fifo_GetLen(&Uart1_RxData_fifo);
    if (RxLength == 0)
    {
        return;
    }

    if (256 >= RxLength)
    {
        fifo_retrieve(&Uart1_RxData_fifo, &Uart1_CurrentRxBuf[0], RxLength);
    }
    else
    {
        fifo_retrieve(&Uart1_RxData_fifo, &Uart1_CurrentRxBuf[0], 256);
        RxLength = 256;
    }

    /*add debug log*/

    /*send　ack*/

    /*Rx data handle , can send to other task*/
    USART1_RxDataHandle(&Uart1_CurrentRxBuf[0], RxLength);
}

void USART1_SendDataManage(void)
{
    //u16 len = 0;
    //u16 SendLen = 0;

    //len = fifo_GetLen(&DispTxData_fifo);

    //if (len == 0)
    {
    //    return;
    }

    //if (len >= DISP_CURRENT_TX_LEN)
    {
        //  SendLen = DISP_CURRENT_TX_LEN;
    }
    //else
    {
        //SendLen = len;
    }

    //fifo_retrieve(&DispTxData_fifo, &DispCurrentTxBuf[0], SendLen);

    //USART1_SendData(&DispCurrentTxBuf[0], SendLen);
}

/*重发数据管理*/
static void USART1_ResendManage(u16 Tick)
{
}

void USART1_ManageProc(u16 Tick)
{
    USART1_ResendManage(Tick);

    USART1_RxDataParse();
}
