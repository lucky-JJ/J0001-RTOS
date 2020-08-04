/*
 * @Author: J
 * @Date: 2020-07-28 13:40:04
 * @LastEditTime: 2020-08-04 17:46:34
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \APP\Codes\Driver\Irq.h
 */

#include "sys.h"
#include "stm32f429xx.h"

#define IRQn_MAX (3)

typedef struct
{
    const IRQn_Type IRQn_Vector;
    u8 PreemptPriority;
    u8 SubPriority;
} IRQn_Config_t;



/** \brief  Enable/Disable Global interrupts
 */
#if defined(__GNUC__)
#define __enable_irq() __asm volatile("cpsie i" \
                                      :         \
                                      :         \
                                      : "memory");
#define __disable_irq() __asm volatile("cpsid i" \
                                       :         \
                                       :         \
                                       : "memory");
#else
#define __enable_irq() __asm("cpsie i")
#define __disable_irq() __asm("cpsid i")
#endif

void IRQ_Init(void);
