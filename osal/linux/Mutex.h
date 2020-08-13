#ifndef OS_RTEMS_MUTEX_H_
#define OS_RTEMS_MUTEX_H_

#include "../../ipc/MutexIF.h"
#include <pthread.h>

class Mutex : public MutexIF {
public:
	Mutex();
	virtual ~Mutex();
	virtual ReturnValue_t lockMutex(uint32_t timeoutMs);
	virtual ReturnValue_t unlockMutex();
private:
	pthread_mutex_t mutex;
	static uint8_t count;
};

#endif /* OS_RTEMS_MUTEX_H_ */
