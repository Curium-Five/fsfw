/**
 * @file TaskManagement.h
 *
 * @date 26.02.2020
 */

#ifndef FRAMEWORK_OSAL_FREERTOS_TASKMANAGEMENT_H_
#define FRAMEWORK_OSAL_FREERTOS_TASKMANAGEMENT_H_

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
};

#endif /* FRAMEWORK_OSAL_FREERTOS_TASKMANAGEMENT_H_ */
