#ifndef FRAMEWORK_OSAL_LINUX_POSIXTHREAD_H_
#define FRAMEWORK_OSAL_LINUX_POSIXTHREAD_H_

#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <framework/returnvalues/HasReturnvaluesIF.h>

class PosixThread {
public:
	PosixThread(const char* name_, int priority_, size_t stackSize_);
	virtual ~PosixThread();
	/**
	 * Set the Thread to sleep state
	 * @param ns Nanosecond sleep time
	 * @return Returns Failed if sleep fails
	 */
	static ReturnValue_t sleep(uint64_t ns);
	/**
	 * @brief Function to suspend the task until SIGUSR1 was received
	 *
	 *  @details Will be called in the beginning to suspend execution until startTask() is called explicitly.
	 */
	void suspend();

	/**
	 * @brief Function to allow a other thread to start the thread again from suspend state
	 *
	 * @details Restarts the Thread after suspend call
	 */
	void resume();


	/**
	 * Delay function similar to FreeRtos delayUntil function
	 *
	 * @param prevoiusWakeTime_ms Needs the previous wake time and returns the next wakeup time
	 * @param delayTime_ms Time period to delay
	 *
	 * @return False If deadline was missed; True if task was delayed
	 */
	static bool delayUntil(uint64_t* const prevoiusWakeTime_ms, const uint64_t delayTime_ms);

	/**
	 * Returns the current time in milliseconds from CLOCK_MONOTONIC
	 *
	 * @return current time in milliseconds from CLOCK_MONOTONIC
	 */
	static uint64_t getCurrentMonotonicTimeMs();

protected:
	pthread_t thread;

	/**
	 * @brief 	Function that has to be called by derived class because the
	 *  		derived class pointer has to be valid as argument
	 * @details
	 * This function creates a pthread with the given parameters. As the
	 * function requires a pointer to the derived object it has to be called
	 * after the this pointer of the derived object is valid.
	 * Sets the taskEntryPoint as function to be called by new a thread.
	 * @param fnc_ Function which will be executed by the thread.
	 * @param arg_
	 * argument of the taskEntryPoint function, needs to be this pointer
	 * of derived class
	 */
	void createTask(void* (*fnc_)(void*),void* arg_);

private:
	char name[16];
	int priority;
	size_t stackSize = 0;
};

#endif /* FRAMEWORK_OSAL_LINUX_POSIXTHREAD_H_ */
