/*
 * @Author: J
 * @Date: 2020-07-28 13:39:44
 * @LastEditTime: 2020-07-28 16:23:33
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \APP\Codes\Driver\Irq.c
 */
#include "Irq.h"

/**
 * @description: 设置中断优先级 ,使能中断
 * @param {type} PreemptPriority: The preemption priority for the IRQn channel.
 *               SubPriority: the subpriority level for the IRQ channel.
 * @return: 
 */
void Irq_EnableVector(IRQn_Type vector, uint32_t PreemptPriority, uint32_t SubPriority)
{
    HAL_NVIC_SetPriority(vector, PreemptPriority, SubPriority);
    HAL_NVIC_EnableIRQ(vector);
}

/**
 * @description: 中断使能初始化
 * @param {type} 
 * @return: 
 */
void IRQ_Init(void)
{
    int32_t Index = 0;

    for (Index = 0; Index < IRQn_MAX; Index++)
    {
        Irq_EnableVector(Irq_Config[Index].IRQn_Vector, Irq_Config[Index].PreemptPriority, Irq_Config[Index].SubPriority);
    }
}

/**
 * @description: 中断管理反初始化
 * @param {type} 
 * @return: 
 */
void IRQ_Deint(void)
{
    int32_t Index = 0;

    for (Index = 0; Index < IRQn_MAX; Index++)
    {
        HAL_NVIC_DisableIRQ(Irq_Config[Index].IRQn_Vector);
    }
}