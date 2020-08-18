#include "../../osal/FreeRTOS/CountingSemaphUsingTask.h"
#include "../../osal/FreeRTOS/TaskManagement.h"
#include "../../serviceinterface/ServiceInterfaceStream.h"

CountingSemaphoreUsingTask::CountingSemaphoreUsingTask(const uint8_t maxCount,
		uint8_t initCount): maxCount(maxCount) {
	if(initCount > maxCount) {
		sif::error << "CountingSemaphoreUsingTask: Max count bigger than "
				"intial cout. Setting initial count to max count." << std::endl;
		initCount = maxCount;
	}

	handle = TaskManagement::getCurrentTaskHandle();
	if(handle == nullptr) {
		sif::error << "CountingSemaphoreUsingTask: Could not retrieve task "
				"handle. Please ensure the constructor was called inside a "
				"task." << std::endl;
	}

	uint32_t oldNotificationValue;
	xTaskNotifyAndQuery(handle, 0, eSetValueWithOverwrite,
			&oldNotificationValue);
	if(oldNotificationValue != 0) {
		sif::warning << "CountinSemaphoreUsingTask: Semaphore initiated but "
				"current notification value is not 0. Please ensure the "
				"notification value is not used for other purposes!" << std::endl;
	}
	for(int i = 0; i < initCount; i++) {
		xTaskNotifyGive(handle);
	}
}

CountingSemaphoreUsingTask::~CountingSemaphoreUsingTask() {
	// Clear notification value on destruction.
	// If this is not desired, don't call the destructor
	// (or implement a boolean which disables the reset)
	xTaskNotifyAndQuery(handle, 0, eSetValueWithOverwrite, nullptr);
}

ReturnValue_t CountingSemaphoreUsingTask::acquire(uint32_t timeoutMs) {
	TickType_t timeout = SemaphoreIF::POLLING;
	if(timeoutMs == SemaphoreIF::BLOCKING) {
		timeout = SemaphoreIF::BLOCKING;
	}
	else if(timeoutMs > SemaphoreIF::POLLING){
		timeout = pdMS_TO_TICKS(timeoutMs);
	}
	return acquireWithTickTimeout(timeout);

}

ReturnValue_t CountingSemaphoreUsingTask::acquireWithTickTimeout(
		TickType_t timeoutTicks) {
	// Decrement notfication value without resetting it.
	BaseType_t oldCount = ulTaskNotifyTake(pdFALSE, timeoutTicks);
	if (getSemaphoreCounter() == oldCount - 1) {
		return HasReturnvaluesIF::RETURN_OK;
	}
	else {
		return SemaphoreIF::SEMAPHORE_TIMEOUT;
	}
}

ReturnValue_t CountingSemaphoreUsingTask::release() {
	if(getSemaphoreCounter() == maxCount) {
		return SemaphoreIF::SEMAPHORE_NOT_OWNED;
	}
	return release(handle);
}

ReturnValue_t CountingSemaphoreUsingTask::release(
		TaskHandle_t taskToNotify) {
	BaseType_t returncode = xTaskNotifyGive(taskToNotify);
	if (returncode == pdPASS) {
		return HasReturnvaluesIF::RETURN_OK;
	}
	else {
		// This should never happen.
		return HasReturnvaluesIF::RETURN_FAILED;
	}
}


uint8_t CountingSemaphoreUsingTask::getSemaphoreCounter() const {
	uint32_t notificationValue = 0;
	xTaskNotifyAndQuery(handle, 0, eNoAction, &notificationValue);
	return notificationValue;
}

TaskHandle_t CountingSemaphoreUsingTask::getTaskHandle() {
	return handle;
}

ReturnValue_t CountingSemaphoreUsingTask::releaseFromISR(
		TaskHandle_t taskToNotify, BaseType_t* higherPriorityTaskWoken) {
	vTaskNotifyGiveFromISR(taskToNotify, higherPriorityTaskWoken);
	return HasReturnvaluesIF::RETURN_OK;
}

uint8_t CountingSemaphoreUsingTask::getSemaphoreCounterFromISR(
		TaskHandle_t task, BaseType_t* higherPriorityTaskWoken) {
	uint32_t notificationValue;
	xTaskNotifyAndQueryFromISR(task, 0, eNoAction, &notificationValue,
			higherPriorityTaskWoken);
	return notificationValue;
}

uint8_t CountingSemaphoreUsingTask::getMaxCount() const {
	return maxCount;
}
