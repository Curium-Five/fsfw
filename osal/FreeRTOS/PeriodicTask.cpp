#include "PeriodicTask.h"

#include <framework/serviceinterface/ServiceInterfaceStream.h>
#include <framework/tasks/ExecutableObjectIF.h>

PeriodicTask::PeriodicTask(const char *name, TaskPriority setPriority,
		TaskStackSize setStack, TaskPeriod setPeriod,
		void (*setDeadlineMissedFunc)()) :
		started(false), handle(NULL), period(setPeriod), deadlineMissedFunc(
		setDeadlineMissedFunc)
{
	BaseType_t status = xTaskCreate(taskEntryPoint, name,
			setStack, this, setPriority, &handle);
	if(status != pdPASS){
		sif::debug << "PeriodicTask Insufficient heap memory remaining. "
		        "Status: " << status << std::endl;
	}

}

PeriodicTask::~PeriodicTask(void) {
	//Do not delete objects, we were responsible for ptrs only.
}

void PeriodicTask::taskEntryPoint(void* argument) {
	// The argument is re-interpreted as PeriodicTask. The Task object is
    // global, so it is found from any place.
	PeriodicTask *originalTask(reinterpret_cast<PeriodicTask*>(argument));
	/* Task should not start until explicitly requested,
	 * but in FreeRTOS, tasks start as soon as they are created if the scheduler
	 * is running but not if the scheduler is not running.
	 * To be able to accommodate both cases we check a member which is set in
	 * #startTask(). If it is not set and we get here, the scheduler was started
	 * before #startTask() was called and we need to suspend if it is set,
	 * the scheduler was not running before #startTask() was called and we
	 * can continue */

	if (not originalTask->started) {
		vTaskSuspend(NULL);
	}

	originalTask->taskFunctionality();
	sif::debug << "Polling task " << originalTask->handle
			<< " returned from taskFunctionality." << std::endl;
}

ReturnValue_t PeriodicTask::startTask() {
	started = true;

	// We must not call resume if scheduler is not started yet
	if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
		vTaskResume(handle);
	}

	return HasReturnvaluesIF::RETURN_OK;
}

ReturnValue_t PeriodicTask::sleepFor(uint32_t ms) {
	vTaskDelay(pdMS_TO_TICKS(ms));
	return HasReturnvaluesIF::RETURN_OK;
}

void PeriodicTask::taskFunctionality() {
	TickType_t xLastWakeTime;
	const TickType_t xPeriod = pdMS_TO_TICKS(this->period * 1000.);
	/* The xLastWakeTime variable needs to be initialized with the current tick
	 count. Note that this is the only time the variable is written to
	 explicitly. After this assignment, xLastWakeTime is updated automatically
	 internally within vTaskDelayUntil(). */
	xLastWakeTime = xTaskGetTickCount();
	/* Enter the loop that defines the task behavior. */
	for (;;) {
		for (auto const& object: objectList) {
			object->performOperation();
		}

		checkMissedDeadline(xLastWakeTime, xPeriod);

		vTaskDelayUntil(&xLastWakeTime, xPeriod);

	}
}

ReturnValue_t PeriodicTask::addComponent(object_id_t object) {
	ExecutableObjectIF* newObject = objectManager->get<ExecutableObjectIF>(
			object);
	if (newObject == nullptr) {
	    sif::error << "PeriodicTask::addComponent: Invalid object. Make sure"
	            "it implements ExecutableObjectIF!" << std::endl;
		return HasReturnvaluesIF::RETURN_FAILED;
	}
	objectList.push_back(newObject);

	newObject->setTaskIF(this);
	return HasReturnvaluesIF::RETURN_OK;
}

uint32_t PeriodicTask::getPeriodMs() const {
	return period * 1000;
}

void PeriodicTask::checkMissedDeadline(const TickType_t xLastWakeTime,
        const TickType_t interval) {
    /* Check whether deadline was missed while also taking overflows
     * into account. Drawing this on paper with a timeline helps to understand
     * it. */
    TickType_t currentTickCount = xTaskGetTickCount();
    TickType_t timeToWake = xLastWakeTime + interval;
    // Tick count has overflown
    if(currentTickCount < xLastWakeTime) {
        // Time to wake has overflown as well. If the tick count
        // is larger than the time to wake, a deadline was missed.
        if(timeToWake < xLastWakeTime and
                currentTickCount > timeToWake) {
            handleMissedDeadline();
        }
    }
    // No tick count overflow. If the timeToWake has not overflown
    // and the current tick count is larger than the time to wake,
    // a deadline was missed.
    else if(timeToWake > xLastWakeTime and currentTickCount > timeToWake) {
        handleMissedDeadline();
    }
}

void PeriodicTask::handleMissedDeadline() {
#ifdef DEBUG
    sif::warning << "PeriodicTask: " << pcTaskGetName(NULL) <<
            " missed deadline!\n" << std::flush;
#endif
    if(deadlineMissedFunc != nullptr) {
        this->deadlineMissedFunc();
    }
}
