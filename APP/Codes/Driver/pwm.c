#include "timer.h"
#include "led.h"
//////////////////////////////////////////////////////////////////////////////////
//修改说明
//V1.1 20160106
//新增TIM3_PWM_Init函数,用于PWM输出
//////////////////////////////////////////////////////////////////////////////////

TIM_HandleTypeDef TIM3_Handler;     //定时器3PWM句柄
TIM_OC_InitTypeDef TIM3_CH4Handler; //定时器3通道4句柄

//TIM3 PWM部分初始化
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM3_PWM_Init(u16 arr, u16 psc)
{
    TIM3_Handler.Instance = TIM3;                       //定时器3
    TIM3_Handler.Init.Prescaler = psc;                  //定时器分频
    TIM3_Handler.Init.CounterMode = TIM_COUNTERMODE_UP; //向上计数模式
    TIM3_Handler.Init.Period = arr;                     //自动重装载值
    TIM3_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&TIM3_Handler); //初始化PWM

    TIM3_CH4Handler.OCMode = TIM_OCMODE_PWM1;                                  //模式选择PWM1
    TIM3_CH4Handler.Pulse = arr / 2;                                           //设置比较值,此值用来确定占空比，默认比较值为自动重装载值的一半,即占空比为50%
    TIM3_CH4Handler.OCPolarity = TIM_OCPOLARITY_LOW;                           //输出比较极性为低
    HAL_TIM_PWM_ConfigChannel(&TIM3_Handler, &TIM3_CH4Handler, TIM_CHANNEL_4); //配置TIM3通道4

    HAL_TIM_PWM_Start(&TIM3_Handler, TIM_CHANNEL_4); //开启PWM通道4
}

//定时器底层驱动，时钟使能，引脚配置
//此函数会被HAL_TIM_PWM_Init()调用
//htim:定时器句柄
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef GPIO_Initure;
    //__HAL_RCC_TIM3_CLK_ENABLE();			//使能定时器3
    //__HAL_RCC_GPIOB_CLK_ENABLE();			//开启GPIOB时钟

    GPIO_Initure.Pin = GPIO_PIN_1;          //PB1
    GPIO_Initure.Mode = GPIO_MODE_AF_PP;    //复用推挽输出
    GPIO_Initure.Pull = GPIO_PULLUP;        //上拉
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;   //高速
    GPIO_Initure.Alternate = GPIO_AF2_TIM3; //PB1复用为TIM3_CH4
    HAL_GPIO_Init(GPIOB, &GPIO_Initure);
}

//设置TIM通道4的占空比
//compare:比较值
void TIM_SetTIM3Compare4(u32 compare)
{
    TIM3->CCR4 = compare;
}
