#include <framework/osal/FreeRTOS/BinSemaphUsingTask.h>
#include <framework/osal/FreeRTOS/TaskManagement.h>


BinarySemaphoreUsingTask::BinarySemaphoreUsingTask() {
	handle = TaskManagement::getCurrentTaskHandle();
}

ReturnValue_t BinarySemaphoreUsingTask::acquire(uint32_t timeoutMs) {
	return takeBinarySemaphore(timeoutMs);
}

ReturnValue_t BinarySemaphoreUsingTask::release() {
	return giveBinarySemaphore();
}

ReturnValue_t BinarySemaphoreUsingTask::takeBinarySemaphore(uint32_t timeoutMs) {
	TickType_t timeout = SemaphoreIF::NO_TIMEOUT;
	if(timeoutMs == SemaphoreIF::MAX_TIMEOUT) {
	    timeout = SemaphoreIF::MAX_TIMEOUT;
	}
	else if(timeoutMs > BinarySemaphoreUsingTask::NO_TIMEOUT){
	    timeout = pdMS_TO_TICKS(timeoutMs);
	}

	BaseType_t returncode = ulTaskNotifyTake(pdTRUE, timeout);
	if (returncode == pdPASS) {
		return HasReturnvaluesIF::RETURN_OK;
	}
	else {
	    return SemaphoreIF::SEMAPHORE_TIMEOUT;
	}
}

ReturnValue_t BinarySemaphoreUsingTask::takeBinarySemaphoreTickTimeout(
        TickType_t timeoutTicks) {
	BaseType_t returncode = ulTaskNotifyTake(pdTRUE, timeoutTicks);
	if (returncode == pdPASS) {
		return HasReturnvaluesIF::RETURN_OK;
	} else {
		return SEMAPHORE_TIMEOUT;
	}
}

ReturnValue_t BinarySemaphoreUsingTask::giveBinarySemaphore() {
	BaseType_t returncode = xTaskNotifyGive(handle);
	if (returncode == pdPASS) {
		return HasReturnvaluesIF::RETURN_OK;
	} else {
		return SEMAPHORE_NOT_OWNED;
	}
}

TaskHandle_t BinarySemaphoreUsingTask::getTaskHandle() {
	return handle;
}

uint8_t BinarySemaphoreUsingTask::getSemaphoreCounter() {
	uint32_t notificationValue;
	xTaskNotifyAndQuery(handle, 0, eNoAction, &notificationValue);
	return notificationValue;
}

uint8_t BinarySemaphoreUsingTask::getSemaphoreCounterFromISR(TaskHandle_t taskHandle) {
	uint32_t notificationValue;
	BaseType_t higherPriorityTaskWoken;
	xTaskNotifyAndQueryFromISR(taskHandle, 0, eNoAction, &notificationValue,
			&higherPriorityTaskWoken);
	if(higherPriorityTaskWoken) {
		TaskManagement::requestContextSwitch(CallContext::isr);
	}
	return notificationValue;
}


ReturnValue_t BinarySemaphoreUsingTask::giveBinarySemaphore(TaskHandle_t taskHandle) {
	BaseType_t returncode = xTaskNotifyGive(taskHandle);
	if (returncode == pdPASS) {
		return HasReturnvaluesIF::RETURN_OK;
	} else {
		return SEMAPHORE_NOT_OWNED;
	}
}

// Be careful with the stack size here. This is called from an ISR!
ReturnValue_t BinarySemaphoreUsingTask::giveBinarySemaphoreFromISR(
		TaskHandle_t taskHandle, BaseType_t * higherPriorityTaskWoken) {
	vTaskNotifyGiveFromISR(taskHandle, higherPriorityTaskWoken);
	if(*higherPriorityTaskWoken == pdPASS) {
		// Request context switch because unblocking the semaphore
		// caused a high priority task unblock.
		TaskManagement::requestContextSwitch(CallContext::isr);
	}
	return HasReturnvaluesIF::RETURN_OK;
}
