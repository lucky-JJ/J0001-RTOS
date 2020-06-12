

#include "sys.h"


typedef struct
{
    Message_t    Msg;
    u8           Reserve;
    u16          Length;
} UART1_DataEvt_t;

typedef struct
{
    UART1_DataEvt_t Head;
    u8           	Data[1];
} UART1_DataEvtSturt;

