#ifndef _KEY_H
#define _KEY_H
#include "sys.h"


//下面的方式是通过直接操作HAL库函数方式读取IO
#define KEY0        HAL_GPIO_ReadPin(GPIOH,GPIO_PIN_3)  //KEY0按键PH3
#define KEY1        HAL_GPIO_ReadPin(GPIOH,GPIO_PIN_2)  //KEY1按键PH2
#define KEY2        HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13) //KEY2按键PC13
#define WK_UP       HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)  //WKUP按键PA0

#define KEY0_PRES 	1
#define KEY1_PRES		2
#define KEY2_PRES		3
#define WKUP_PRES   4

#define KEY_NUMBER 4

u8 const Key[KEYNUMBER] = \
{
	GPIO_Read(IO_KEY0),
	GPIO_Read(IO_KEY1),
	GPIO_Read(IO_KEY2),
	GPIO_Read(IO_WK_UP),
}

typedef enum
{
    KEY_IDLE,
    KEY_PRESS,
    KEY_SINGLE,
    KEY_LONGPRESS,
    KEY_LONGPRESS_CONTINUE,
    KEY_RELEASE,
} Enum_Key_State;

typedef struct {
    Message_t   Msg;
    u8          Key0:2;
	u8          Key1:2;
	u8			Key2:2;
	u8          Key_WK_UP:2;
} KeyData_t;



u8 KEY_Scan(u8 mode);

void KeyStatusCheckProc(void);


#endif
