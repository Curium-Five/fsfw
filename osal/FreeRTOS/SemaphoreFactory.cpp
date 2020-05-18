#include <framework/osal/FreeRTOS/BinarySemaphore.h>
#include <framework/osal/FreeRTOS/CountingSemaphore.h>
#include <framework/tasks/SemaphoreFactory.h>
#include <framework/serviceinterface/ServiceInterfaceStream.h>

SemaphoreFactory* SemaphoreFactory::factoryInstance = nullptr;

SemaphoreFactory::SemaphoreFactory() {
}

SemaphoreFactory::~SemaphoreFactory() {
	delete factoryInstance;
}

SemaphoreFactory* SemaphoreFactory::instance() {
	if (factoryInstance == nullptr){
		factoryInstance = new SemaphoreFactory();
	}
	return SemaphoreFactory::factoryInstance;
}

SemaphoreIF* SemaphoreFactory::createBinarySemaphore() {
	return new Semaphore();
}

SemaphoreIF* SemaphoreFactory::createCountingSemaphore(uint8_t count,
		uint8_t initCount) {
	return new CountingSemaphore(count, initCount);
}

void SemaphoreFactory::deleteMutex(SemaphoreIF* semaphore) {
	delete semaphore;
}
