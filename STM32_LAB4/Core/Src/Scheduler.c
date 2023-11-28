/*
 * Scheduler.c
 *
 *  Created on: Nov 15, 2023
 *      Author: DELL
 */
#include "Scheduler.h"

uint32_t current_task_index;
sTask SCH_tasks_G[SCH_MAX_TASKS];

void SCH_Init(){
	current_task_index = 0;
}

void SCH_Update(void){
	if(SCH_tasks_G[0].pTask && SCH_tasks_G[0].RunMe == 0){
		if(SCH_tasks_G[0].Delay > 0){
			// count down to 0 to run task
			SCH_tasks_G[0].Delay--;
		}
		if(SCH_tasks_G[0].Delay == 0){
			// run this task
			SCH_tasks_G[0].RunMe = 1;
		}
	}
}

void SCH_Dispatch_Tasks(void){
	if(SCH_tasks_G[0].RunMe > 0){
		// run task
		(*SCH_tasks_G[0].pTask)();
		// restore the function and period of this task
		void(*pFunction)() = SCH_tasks_G[0].pTask;
		uint32_t period = SCH_tasks_G[0].Period;
		// remove it (head of queue)
		SCH_Delete_Task(0);
		// add it again to queue (arrange by delay)
		SCH_Add_Task(pFunction, period, period);
	}
}

void SCH_Add_Task(void(*pFunction)(),
		uint32_t DELAY, uint32_t PERIOD){
	if(current_task_index < SCH_MAX_TASKS){
		// not accept oneshot task
		if(PERIOD == 0) return;

		uint32_t i = 0;
		while(DELAY >= SCH_tasks_G[i].Delay && i < current_task_index){
			DELAY -= SCH_tasks_G[i].Delay;
			i++;
		}
		/* from now DELAY mean that this is a countdown time to
		 * execution of the current task compared to the previous
		 * task
		 */

		/* now Add_Task function will sort the task queue by the
		 * DELAY time from smallest to biggest
		 */
		if(i != current_task_index){
			//insert new task into this place (i-1)th ele
			for(uint32_t idx = current_task_index; idx > i; idx--){
				SCH_tasks_G[idx].pTask = SCH_tasks_G[idx - 1].pTask;
				SCH_tasks_G[idx].Delay = SCH_tasks_G[idx - 1].Delay;
				SCH_tasks_G[idx].Period = SCH_tasks_G[idx - 1].Period;
				SCH_tasks_G[idx].RunMe= SCH_tasks_G[idx - 1].RunMe;
				SCH_tasks_G[idx].TaskID = idx;
			}
			// update delay time of next task
			SCH_tasks_G[i+1].Delay -= DELAY;
		}
		SCH_tasks_G[i].pTask = pFunction;
		SCH_tasks_G[i].Delay = DELAY;
		SCH_tasks_G[i].Period = PERIOD;
		SCH_tasks_G[i].RunMe = 0;
		SCH_tasks_G[i].TaskID = i;
		current_task_index++;
	}
}

void SCH_Delete_Task(uint32_t taskID){
	for(uint32_t i = taskID; i < current_task_index; i ++){
		SCH_tasks_G[i].pTask = SCH_tasks_G[i+1].pTask;
		SCH_tasks_G[i].Delay = SCH_tasks_G[i+1].Delay;
		SCH_tasks_G[i].Period = SCH_tasks_G[i+1].Period;
		SCH_tasks_G[i].RunMe = SCH_tasks_G[i+1].RunMe;
		SCH_tasks_G[i].TaskID = i;
	}
	current_task_index--;
}

