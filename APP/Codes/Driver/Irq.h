/*
 * @Author: J
 * @Date: 2020-07-28 13:40:04
 * @LastEditTime: 2020-07-29 09:07:05
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \APP\Codes\Driver\Irq.h
 */

#include "sys.h"

#define IRQn_MAX (3)

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

typedef struct
{
    const IRQn_Type IRQn_Vector;
    u8 PreemptPriority;
    u8 SubPriority;
} IRQn_Config_t;

const IRQn_Config_t Irq_Config[IRQn_MAX] =
    {
        (USART1_IRQn, 0x03, 0x03),
        (RTC_Alarm_IRQn, 0x01, 0x02),
        (RTC_WKUP_IRQn, 0x02, 0x02),
};

void IRQ_Init(void);