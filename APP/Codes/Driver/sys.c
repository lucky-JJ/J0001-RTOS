#include "sys.h"
#include "task.h"

//////////////////////////////////////////////////////////////////////////////////	 
////////////////////////////////////////////////////////////////////////////////// 

//时钟系统配置函数
//Fvco=Fs*(plln/pllm);
//SYSCLK=Fvco/pllp=Fs*(plln/(pllm*pllp));
//Fusb=Fvco/pllq=Fs*(plln/(pllm*pllq));

//Fvco:VCO频率
//SYSCLK:系统时钟频率
//Fusb:USB,SDIO,RNG等的时钟频率
//Fs:PLL输入时钟频率,可以是HSI,HSE等. 
//plln:主PLL倍频系数(PLL倍频),取值范围:64~432.
//pllm:主PLL和音频PLL分频系数(PLL之前的分频),取值范围:2~63.PLL VCO输入时钟的除法因子
//pllp:系统时钟的主PLL分频系数(PLL之后的分频),取值范围:2,4,6,8.(仅限这4个值!)主系统时钟的除法因子(SYSCLK)
//pllq:USB/SDIO/随机数产生器等的主PLL分频系数(PLL之后的分频),取值范围:2~15.

/*
STM32 有4个独立时钟源:HSI、HSE、LSI、LSE。
①、HSI是高速内部时钟，RC振荡器，频率为8MHz，精度不高。
②、HSE是高速外部时钟，可接石英/陶瓷谐振器，或者接外部时钟源，频率范围为4MHz~16MHz。
③、LSI是低速内部时钟，RC振荡器，频率为40kHz，提供低功耗时钟。　 
④、LSE是低速外部时钟，接频率为32.768kHz的石英晶体。

其中LSI是作为IWDGCLK(独立看门狗)时钟源和RTC时钟源 而独立使用 

而HSI高速内部时钟 HSE高速外部时钟 LSI低速内部时钟  这三个经过分频或者倍频 作为系统时钟来使用
系统时钟经过 pll 锁相环倍频输出为pll时钟,链接到AHB，其pll时钟输入源可选择为HSI/2、HSE或者HSE/2。倍频可选择为2~16倍

*/

//外部晶振为25M的时候,推荐值:plln=360,pllm=25,pllp=2,pllq=8.
//得到:Fvco=25*(360/25)=360Mhz
//     SYSCLK=360/2=180Mhz
//     Fusb=360/8=45Mhz
//返回值:0,成功;1,失败  
void Clock_Init(void)
{
    HAL_StatusTypeDef ret = HAL_OK;
    RCC_OscInitTypeDef RCC_OscInitStructure; 
    RCC_ClkInitTypeDef RCC_ClkInitStructure;
    
    __HAL_RCC_PWR_CLK_ENABLE(); //使能PWR时钟
    
    //下面这个设置用来设置调压器输出电压级别，以便在器件未以最大频率工作
    //时使性能与功耗实现平衡，此功能只有STM32F42xx和STM32F43xx器件有，
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);//设置调压器输出电压级别1
    
    RCC_OscInitStructure.OscillatorType=RCC_OSCILLATORTYPE_HSE;    //时钟源为HSE
    RCC_OscInitStructure.HSEState=RCC_HSE_ON;                      //打开HSE
    RCC_OscInitStructure.PLL.PLLState=RCC_PLL_ON;//打开PLL
    RCC_OscInitStructure.PLL.PLLSource=RCC_PLLSOURCE_HSE;//PLL时钟源选择HSE
    RCC_OscInitStructure.PLL.PLLM = 25; //主PLL和音频PLL分频系数(PLL之前的分频),取值范围:2~63.
    RCC_OscInitStructure.PLL.PLLN = 360; //主PLL倍频系数(PLL倍频),取值范围:64~432.  
    RCC_OscInitStructure.PLL.PLLP = 2; //系统时钟的主PLL分频系数(PLL之后的分频),取值范围:2,4,6,8.(仅限这4个值!)
    RCC_OscInitStructure.PLL.PLLQ = 8; //USB/SDIO/随机数产生器等的主PLL分频系数(PLL之后的分频),取值范围:2~15.
    ret=HAL_RCC_OscConfig(&RCC_OscInitStructure);//初始化
	
    if(ret!=HAL_OK) while(1);
    
    ret=HAL_PWREx_EnableOverDrive(); //开启Over-Driver功能,超频必须开
    if(ret!=HAL_OK) while(1);
    
    //选中PLL作为系统时钟源并且配置HCLK,PCLK1和PCLK2
    RCC_ClkInitStructure.ClockType=(RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStructure.SYSCLKSource=RCC_SYSCLKSOURCE_PLLCLK;//设置系统时钟时钟源为PLL
    RCC_ClkInitStructure.AHBCLKDivider=RCC_SYSCLK_DIV1;//AHB分频系数为1
    RCC_ClkInitStructure.APB1CLKDivider=RCC_HCLK_DIV4; //APB1分频系数为4
    RCC_ClkInitStructure.APB2CLKDivider=RCC_HCLK_DIV2; //APB2分频系数为2
    ret=HAL_RCC_ClockConfig(&RCC_ClkInitStructure,FLASH_LATENCY_5);//同时设置FLASH延时周期为5WS，也就是6个CPU周期。
		
    if(ret!=HAL_OK) while(1);
}


void IRQ_Manage(void)
{
	/*外设时钟必须在初始化外设之前配置*/

	/*uart1*/
	__HAL_RCC_USART1_CLK_ENABLE();		//使能USART1时钟
	
	HAL_NVIC_EnableIRQ(USART1_IRQn);	//使能USART1中断通道
	HAL_NVIC_SetPriority(USART1_IRQn,3,3); //抢占优先级3，子优先级3

	/*SPI*/
	__HAL_RCC_SPI5_CLK_ENABLE();

}



extern void xPortSysTickHandler(void);
//systick中断服务函数,使用OS时用到,嘀嗒定时器 ,1ms 进入一次
void SysTick_Handler(void)
{  
    if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)//系统已经运行
    {
        xPortSysTickHandler();	
    }
    HAL_IncTick();
}
	

/**/

typedef struct
{
	u8  CtrlType;
	u32 CtrlTime;

}ctrl_TypeDef;

ctrl_TypeDef ctrl_t;



static void UpdateTimingStart(u8 ctrlType)
{
    g_SystemTime = HAL_GetTick();
}

static void App_UpdateCtrlCondition(u8 ctrlType)
{
    ctrl_t.CtrlType = ctrlType;
    ctrl_t.CtrlTime = HAL_GetTick();
}


#define TIMER_TIME_DIFF(later, earlier) (((u32)earlier>(u32)later) ? ((0xFFFFFFFFu-(u32)earlier)+(u32)later+1):((u32)later-(u32)earlier) )


//TIMER_TIME_DIFF(HAL_GetTick(), ctrl_t.CtrlTime)>=1000


static void App_RemoteCtrlResp(u8 ucCtrlByte, u8 ucResult, u8 ucFailedCode)
{
	//sendmsg();
    //App_UpdateCtrlCondition(rmt_ctrl_none);   //远程控制结清除控制
}








#ifdef  USE_FULL_ASSERT
//当编译提示出错的时候此函数用来报告错误的文件和所在行
//file：指向源文件
//line：指向在文件中的行数
void assert_failed(uint8_t* file, uint32_t line)
{ 
	while (1)
	{
	}
}
#endif

//THUMB指令不支持汇编内联
//采用如下方法实现执行汇编指令WFI  
__asm void WFI_SET(void)
{
	WFI;		  
}
//关闭所有中断(但是不包括fault和NMI中断)
__asm void INTX_DISABLE(void)
{
	CPSID   I
	BX      LR	  
}
//开启所有中断
__asm void INTX_ENABLE(void)
{
	CPSIE   I
	BX      LR  
}
//设置栈顶地址
//addr:栈顶地址
__asm void MSR_MSP(u32 addr) 
{
	MSR MSP, r0 			//set Main Stack value
	BX r14
}
