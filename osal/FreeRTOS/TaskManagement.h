#ifndef FRAMEWORK_OSAL_FREERTOS_TASKMANAGEMENT_H_
#define FRAMEWORK_OSAL_FREERTOS_TASKMANAGEMENT_H_

#include <framework/returnvalues/HasReturnvaluesIF.h>

extern "C" {
#include "FreeRTOS.h"
#include "task.h"
}
#include <cstdint>

/**
 * Architecture dependant portmacro.h function call.
 * Should be implemented in bsp.
 */
extern "C" void requestContextSwitchFromISR();

/*!
 * Used by functions to tell if they are being called from
 * within an ISR or from a regular task. This is required because FreeRTOS
 * has different functions for handling semaphores and messages from within an ISR and task.
 */

enum CallContext {
	task = 0x00,//!< task_context
	isr = 0xFF  //!< isr_context
};


class TaskManagement {
public:
	/**
	 * In this function, a function dependant on the portmacro.h header function calls
	 * to request a context switch can be specified.
	 * This can be used if sending to the queue from an ISR caused a task to unblock
	 * and a context switch is required.
	 */
	static void requestContextSwitch(CallContext callContext);

	/**
	 * If task preemption in FreeRTOS is disabled, a context switch
	 * can be requested manually by calling this function.
	 */
	static void requestContextSwitchFromTask(void);

	/**
	 * @return The current task handle
	 */
	static TaskHandle_t getCurrentTaskHandle();

	/**
	 * Get returns the minimum amount of remaining stack space in words
	 * that was a available to the task since the task started executing.
	 * Please note that the actual value in bytes depends
	 * on the stack depth type.
	 * E.g. on a 32 bit machine, a value of 200 means 800 bytes.
	 * @return Smallest value of stack remaining since the task was started in
	 * 		   words.
	 */
	static configSTACK_DEPTH_TYPE getTaskStackHighWatermark();

	/**
	 * Function to be called to delay current task
	 * @param delay The delay in milliseconds
	 * @return Success of deletion
	 */
	static ReturnValue_t delayTask(uint32_t delayMs);
};

#endif /* FRAMEWORK_OSAL_FREERTOS_TASKMANAGEMENT_H_ */
