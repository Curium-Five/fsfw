#include <framework/osal/FreeRTOS/CountingSemaphore.h>
#include <framework/serviceinterface/ServiceInterfaceStream.h>
#include <framework/osal/FreeRTOS/TaskManagement.h>

extern "C" {
#include <freertos/semphr.h>
}

// Make sure #define configUSE_COUNTING_SEMAPHORES 1 is set in
// free FreeRTOSConfig.h file.
CountingSemaphore::CountingSemaphore(const uint8_t maxCount, uint8_t initCount):
		maxCount(maxCount), initCount(initCount) {
	handle = xSemaphoreCreateCounting(maxCount, initCount);
	if(handle == nullptr) {
		sif::error << "CountingSemaphore: Creation failure" << std::endl;
	}
}

CountingSemaphore::CountingSemaphore(CountingSemaphore&& other):
		maxCount(other.maxCount), initCount(other.initCount) {
	handle = xSemaphoreCreateCounting(other.maxCount, other.initCount);
	if(handle == nullptr) {
		sif::error << "CountingSemaphore: Creation failure" << std::endl;
	}
}

CountingSemaphore& CountingSemaphore::operator =(
		CountingSemaphore&& other) {
	handle = xSemaphoreCreateCounting(other.maxCount, other.initCount);
	if(handle == nullptr) {
		sif::error << "CountingSemaphore: Creation failure" << std::endl;
	}
	return * this;
}

