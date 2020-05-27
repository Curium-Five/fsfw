#ifndef FRAMEWORK_OSAL_FREERTOS_COUNTINGSEMAPHUSINGTASK_H_
#define FRAMEWORK_OSAL_FREERTOS_COUNTINGSEMAPHUSINGTASK_H_

#include <framework/osal/FreeRTOS/CountingSemaphUsingTask.h>
#include <framework/tasks/SemaphoreIF.h>

extern "C" {
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
}

class CountingSemaphoreUsingTask: public SemaphoreIF {
public:
	CountingSemaphoreUsingTask(uint8_t count, uint8_t initCount);

	ReturnValue_t acquire(uint32_t timeoutMs) override;
	ReturnValue_t release() override;
	uint8_t getSemaphoreCounter() override;

private:
	TaskHandle_t handle;
	uint8_t count = 0;
	uint8_t initCount = 0;
};

#endif /* FRAMEWORK_OSAL_FREERTOS_COUNTINGSEMAPHUSINGTASK_H_ */
