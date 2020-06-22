#include "key.h"
#include "delay.h"
#include "MsgDefine.h"

static u8 	KeyState = KEY_IDLE;
static u8 	KeyCurState = 0xFF;
static u8 	KeyLastState = 0xFF;
static u16 	keycount = 0;



//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，WKUP按下 WK_UP
//注意此函数有响应优先级,KEY0>KEY1>KEY2>WK_UP!!
u8 KEY_Scan(u8 mode)
{
    static u8 key_up=1;     //按键松开标志
    if(mode==1)key_up=1;    //支持连按
    if(key_up&&(KEY0==0||KEY1==0||KEY2==0||WK_UP==1))
    {
        delay_ms(10);
        key_up=0;
        if(KEY0==0)       return KEY0_PRES;
        else if(KEY1==0)  return KEY1_PRES;
        else if(KEY2==0)  return KEY2_PRES;
        else if(WK_UP==1) return WKUP_PRES;          
    }else if(KEY0==1&&KEY1==1&&KEY2==1&&WK_UP==0)key_up=1;
    return 0;   //无按键按下
}

static u8 GetKeyState(u8 state)
{
	switch (KeyState)
	{
		case KEY_IDLE:
			 if(state == 1)
			 {					   
			   keycount++;
			   if(keycount >= 5)
			   {
				   KeyState =KEY_PRESS;
				   keycount = 0;				 
				}					 
			 }
		break;

		 case KEY_PRESS:
			   if(state == 0)
			   {
				   keycount++;
					if(keycount >= 5)
					{
						KeyState =KEY_RELEASE;
						keycount = 0;
					
					}
			   }
		 break;

		 case KEY_RELEASE:
			 KeyState = KEY_IDLE;

		 break;
		 default:			 
			 KeyState = KEY_IDLE;
			 break;
	}

    if(gucAvnKeyStat == KEY_PRESS)
    {
        KeyCurState = 1;
    }
    else if(gucAvnKeyStat == KEY_RELEASE)
    {
        KeyCurState = 0;
    }

	return 0;
}




void KeyStatusCheckProc(void)
{
	u8 i = 0;
				KeyData_t  KeyReqMsg;

			KeyReqMsg.Type = EVENT_KEY_SWITCH_REQ;
	
	for(i = 0; i < KEY_NUMBER ; i++)
	{
		GetKeyState(Key[i]);
	
	

		if(KeyCurState[i] != KeyLastState[i] && KeyCurState[i] != 0xFF)
		{

			KeyReqMsg.Key0
			SendMsgToLedManage((u8 *)&KeyReqMsg, sizeof(Message_t));
			//DEBUG_MCU(DBG_INFO, VEH_MODULE_ID, "AVN Key Press\r\n");
			
			KeyLastState[i] = KeyCurState[i];
		}

	}
}


































