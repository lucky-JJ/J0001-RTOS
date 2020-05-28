#ifndef __APP_CFG
#define __APP_CFG     // prevent multiple invocation

#define TASK_CREATE(TaskName) \
		void TaskName##_Create(void)

#define TASK_INIT(TaskName) \
		void TaskName##_Init(void)

#undef _TSK_CFG_
#undef STDTASKDEF
#define STDTASKDEF(osName, Mode, Prio, stack, dtqdepth)  \
			extern TASK_INIT(osName);
#include "tsk.h"
#undef STDTASKDEF

#undef _TSK_CFG_
#undef STDTASKDEF
#define STDTASKDEF(osName, Mode, Prio, stack, dtqdepth)  \
			extern TASK_CREATE(osName);
#include "tsk.h"
#undef STDTASKDEF


#endif   // __APP_CFG
