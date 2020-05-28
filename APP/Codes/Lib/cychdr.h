/******************************************************************************
 * Project          ChangAn S111
 * (c) copyright    2017
 * Company          chinatsp
 *                  All rights reserved
 *  Date            05.04.2017
 ******************************************************************************/
/**
 *  @file           Cychdr.h
 *  @ingroup        framwork
 *  @author         huangtao
 *
 ******************************************************************************/


#include "osal.h"
#include "eventDefine.h"
#include "sys.h"
#include "TaskId.h"



#define CYCHDR_TICK            5

#define CYCLE_TIMER_EVNET_CFG                                                                             \
	{false , 1 , 5  , TASK_NAME(Key), 	EVENT_Key_TIMER_5MS_EVENT   },\
	{false , 1 , 10 , TASK_NAME(Key), 	EVENT_Key_TIMER_10MS_EVENT  },\
	{false , 1 , 5  , TASK_NAME(Led), 	EVENT_Led_TIMER_5MS_EVENT   },\
	{false , 1 , 20 , TASK_NAME(Led), 	EVENT_Led_TIMER_20MS_EVENT  },\
	{false , 1 , 5  , TASK_NAME(Uart1),	EVENT_Uart1_TIMER_5MS_EVENT },


extern void enableCycleEvent(u8 tskId);

extern void disableCycleEvent(u8 tskId);

void cycleTaskTick(void);


void cycleTaskInit(void);

typedef long (*syscall_func)();

/* system call table */
struct finsh_syscall
{
	const char*		name;		/* the name of system call */
	syscall_func    func;		/* the function address of system call */
};

/* system call item */
struct finsh_syscall_item
{
	struct finsh_syscall_item* next;	/* next item */
	struct finsh_syscall syscall;		/* syscall */
};

extern struct finsh_syscall *_syscall_table_begin, *_syscall_table_end;
extern struct finsh_syscall_item *global_syscall_list;

/* find out system call, which should be implemented in user program */
struct finsh_syscall* finsh_syscall_lookup(const char* name);

/* system variable table */
struct finsh_sysvar
{
	const char*		name;		/* the name of variable */
	u8		        type;		/* the type of variable */
	void*		    var ;		/* the address of variable */
};

#if defined(_MSC_VER) || (defined(__GNUC__) && defined(__x86_64__))
struct finsh_syscall* finsh_syscall_next(struct finsh_syscall* call);
struct finsh_sysvar* finsh_sysvar_next(struct finsh_sysvar* call);
#define FINSH_NEXT_SYSCALL(index)  index=finsh_syscall_next(index)
#define FINSH_NEXT_SYSVAR(index)   index=finsh_sysvar_next(index)
#else
#define FINSH_NEXT_SYSCALL(index)  index++
#define FINSH_NEXT_SYSVAR(index)   index++
#endif

#define FINSH_FUNCTION_EXPORT_CMD(name, cmd, desc)      \
    const char __fsym_##cmd##_name[] = #cmd;            \
    const struct finsh_syscall __fsym_##cmd =           \
    {                           \
        __fsym_##cmd##_name,    \
        (syscall_func)&name     \
    }

#define FINSH_VAR_EXPORT(name, type, desc)              \
    const char __vsym_##name##_name[] = #name;          \
    const struct finsh_sysvar __vsym_##name = \
    {                           \
        __vsym_##name##_name,   \
        type,                   \
        (void*)&name            \
    }

/**
 * @ingroup finsh
 *
 * This macro exports a system function to finsh shell.
 *
 * @param name the name of function.
 * @param desc the description of function, which will show in help.
 */
#define FINSH_FUNCTION_EXPORT(name, desc)   \
    FINSH_FUNCTION_EXPORT_CMD(name, name, desc)

/**
 * @ingroup finsh
 *
 * This macro exports a system function with an alias name to finsh shell.
 *
 * @param name the name of function.
 * @param alias the alias name of function.
 * @param desc the description of function, which will show in help.
 */
#define FINSH_FUNCTION_EXPORT_ALIAS(name, alias, desc)  \
        FINSH_FUNCTION_EXPORT_CMD(name, alias, desc)

#define MSH_CMD_EXPORT(command, desc)   \
        FINSH_FUNCTION_EXPORT_CMD(command, __cmd_##command, desc)

