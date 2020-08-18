#ifndef FRAMEWORK_OSAL_FREERTOS_COUNTINGSEMAPHUSINGTASK_H_
#define FRAMEWORK_OSAL_FREERTOS_COUNTINGSEMAPHUSINGTASK_H_

#include "../../osal/FreeRTOS/CountingSemaphUsingTask.h"
#include "../../tasks/SemaphoreIF.h"

extern "C" {
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
}

/**
 * @brief 	Couting Semaphore implementation which uses the notification value
 * 			of the task. The notification value should therefore not be used
 * 			for other purposes.
 * @details
 *  Additional information: https://www.freertos.org/RTOS-task-notifications.html
 *  and general semaphore documentation.
 */
class CountingSemaphoreUsingTask: public SemaphoreIF {
public:
	CountingSemaphoreUsingTask(const uint8_t maxCount, uint8_t initCount);
	virtual ~CountingSemaphoreUsingTask();

	/**
	 * Acquire the counting semaphore.
	 * If no semaphores are available, the task will be blocked
	 * for a maximum of #timeoutMs or until one is given back,
	 * for example by an ISR or another task.
	 * @param timeoutMs
	 * @return -@c RETURN_OK on success
	 *         -@c SemaphoreIF::SEMAPHORE_TIMEOUT on timeout
	 */
	ReturnValue_t acquire(uint32_t timeoutMs = SemaphoreIF::BLOCKING) override;

	/**
	 * Release a semaphore, increasing the number of available counting
	 * semaphores up to the #maxCount value.
	 * @return -@c RETURN_OK on success
	 *         -@c SemaphoreIF::SEMAPHORE_NOT_OWNED if #maxCount semaphores are
	 *         	already available.
	 */
	ReturnValue_t release() override;

	uint8_t getSemaphoreCounter() const override;
	/**
	 * Get the semaphore counter from an ISR.
	 * @param task
	 * @param higherPriorityTaskWoken This will be set to pdPASS if a task with
	 * a higher priority was unblocked. A context switch should be requested
	 * from an ISR if this is the case (see TaskManagement functions)
	 * @return
	 */
	static uint8_t getSemaphoreCounterFromISR(TaskHandle_t task,
			BaseType_t* higherPriorityTaskWoken);

	/**
	 * Acquire with a timeout value in ticks
	 * @param timeoutTicks
	 * @return -@c RETURN_OK on success
	 *         -@c SemaphoreIF::SEMAPHORE_TIMEOUT on timeout
	 */
	ReturnValue_t acquireWithTickTimeout(
			TickType_t timeoutTicks = SemaphoreIF::BLOCKING);

	/**
	 * Get handle to the task related to the semaphore.
	 * @return
	 */
	TaskHandle_t getTaskHandle();

	/**
	 * Release semaphore of task by supplying task handle
	 * @param taskToNotify
	 * @return -@c RETURN_OK on success
	 *         -@c SemaphoreIF::SEMAPHORE_NOT_OWNED if #maxCount semaphores are
	 *         	already available.
	 */
	static ReturnValue_t release(TaskHandle_t taskToNotify);
	/**
	 * Release seamphore of a task from an ISR.
	 * @param taskToNotify
	 * @param higherPriorityTaskWoken This will be set to pdPASS if a task with
	 * a higher priority was unblocked. A context switch should be requested
	 * from an ISR if this is the case (see TaskManagement functions)
	 * @return -@c RETURN_OK on success
	 *         -@c SemaphoreIF::SEMAPHORE_NOT_OWNED if #maxCount semaphores are
	 *         	already available.
	 */
	static ReturnValue_t releaseFromISR(TaskHandle_t taskToNotify,
			BaseType_t* higherPriorityTaskWoken);

	uint8_t getMaxCount() const;

private:
	TaskHandle_t handle;
	const uint8_t maxCount;
};

#endif /* FRAMEWORK_OSAL_FREERTOS_COUNTINGSEMAPHUSINGTASK_H_ */
