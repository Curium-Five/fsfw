#include "PosixThread.h"

#include "../../serviceinterface/ServiceInterface.h"

#include <cstring>
#include <errno.h>

PosixThread::PosixThread(const char* name_, int priority_, size_t stackSize_):
		thread(0), priority(priority_), stackSize(stackSize_) {
    name[0] = '\0';
    std::strncat(name, name_, PTHREAD_MAX_NAMELEN - 1);
}

PosixThread::~PosixThread() {
    //No deletion and no free of Stack Pointer
}

ReturnValue_t PosixThread::sleep(uint64_t ns) {
    //TODO sleep might be better with timer instead of sleep()
    timespec time;
    time.tv_sec = ns/1000000000;
    time.tv_nsec = ns - time.tv_sec*1e9;

    //Remaining Time is not set here
    int status = nanosleep(&time,NULL);
    if(status != 0){
        switch(errno){
        case EINTR:
            //The nanosleep() function was interrupted by a signal.
            return HasReturnvaluesIF::RETURN_FAILED;
        case EINVAL:
            //The rqtp argument specified a nanosecond value less than zero or
            // greater than or equal to 1000 million.
            return HasReturnvaluesIF::RETURN_FAILED;
        default:
            return HasReturnvaluesIF::RETURN_FAILED;
        }

    }
    return HasReturnvaluesIF::RETURN_OK;
}

void PosixThread::suspend() {
    //Wait for SIGUSR1
    int caughtSig = 0;
    sigset_t waitSignal;
    sigemptyset(&waitSignal);
    sigaddset(&waitSignal, SIGUSR1);
    sigwait(&waitSignal, &caughtSig);
    if (caughtSig != SIGUSR1) {
#if FSFW_CPP_OSTREAM_ENABLED == 1
        sif::error << "FixedTimeslotTask: Unknown Signal received: " <<
                caughtSig << std::endl;
#endif
    }
}

void PosixThread::resume(){
    /* Signal the thread to start. Makes sense to call kill to start or? ;)
    According to POSIX raise(signal) will call pthread_kill(pthread_self(), sig),
    but as the call must be done from the thread itself this is not possible here */
    pthread_kill(thread,SIGUSR1);
}

bool PosixThread::delayUntil(uint64_t* const prevoiusWakeTime_ms,
		const uint64_t delayTime_ms) {
	uint64_t nextTimeToWake_ms;
	bool shouldDelay = false;
	//Get current Time
	const uint64_t currentTime_ms = getCurrentMonotonicTimeMs();
	/* Generate the tick time at which the task wants to wake. */
	nextTimeToWake_ms = (*prevoiusWakeTime_ms) + delayTime_ms;

	if (currentTime_ms < *prevoiusWakeTime_ms) {
		/* The tick count has overflowed since this function was
		lasted called.  In this case the only time we should ever
		actually delay is if the wake time has also overflowed,
		and the wake time is greater than the tick time. When this
		is the case it is as if neither time had overflowed. */
		if ((nextTimeToWake_ms < *prevoiusWakeTime_ms)
				&& (nextTimeToWake_ms > currentTime_ms)) {
			shouldDelay = true;
		}
	} else {
		/* The tick time has not overflowed. In this case we will
		delay if either the wake time has overflowed, and/or the
		tick time is less than the wake time. */
		if ((nextTimeToWake_ms < *prevoiusWakeTime_ms)
				|| (nextTimeToWake_ms > currentTime_ms)) {
			shouldDelay = true;
		}
	}

    /* Update the wake time ready for the next call. */

    (*prevoiusWakeTime_ms) = nextTimeToWake_ms;

    if (shouldDelay) {
        uint64_t sleepTime = nextTimeToWake_ms - currentTime_ms;
        PosixThread::sleep(sleepTime * 1000000ull);
        return true;
    }
    /* We are shifting the time in case the deadline was missed like RTEMS */
    (*prevoiusWakeTime_ms) = currentTime_ms;
    return false;

}


uint64_t PosixThread::getCurrentMonotonicTimeMs(){
    timespec timeNow;
    clock_gettime(CLOCK_MONOTONIC_RAW, &timeNow);
    uint64_t currentTime_ms = (uint64_t) timeNow.tv_sec * 1000
            + timeNow.tv_nsec / 1000000;

    return currentTime_ms;
}


void PosixThread::createTask(void* (*fnc_)(void*), void* arg_) {
#if FSFW_CPP_OSTREAM_ENABLED == 1
    //sif::debug << "PosixThread::createTask" << std::endl;
#endif
    /*
     * The attr argument points to a pthread_attr_t structure whose contents
     * are used at thread creation time to determine attributes for the new
     * thread; this structure is initialized using pthread_attr_init(3) and
     * related functions.  If attr is NULL, then the thread is created with
     * default attributes.
     */
    pthread_attr_t attributes;
    int status = pthread_attr_init(&attributes);
    if(status != 0){
#if FSFW_CPP_OSTREAM_ENABLED == 1
        sif::error << "Posix Thread attribute init failed with: " <<
                strerror(status) << std::endl;
#endif
    }
    void* stackPointer;
    status = posix_memalign(&stackPointer, sysconf(_SC_PAGESIZE), stackSize);
    if(status != 0){
#if FSFW_CPP_OSTREAM_ENABLED == 1
        sif::error << "PosixThread::createTask: Stack init failed with: " <<
                strerror(status) << std::endl;
#endif
        if(errno == ENOMEM) {
            size_t stackMb = stackSize/10e6;
#if FSFW_CPP_OSTREAM_ENABLED == 1
            sif::error << "PosixThread::createTask: Insufficient memory for"
                    " the requested " << stackMb << " MB" << std::endl;
#else
            sif::printError("PosixThread::createTask: Insufficient memory for "
                    "the requested %lu MB\n", static_cast<unsigned long>(stackMb));
#endif
        }
        else if(errno == EINVAL) {
#if FSFW_CPP_OSTREAM_ENABLED == 1
            sif::error << "PosixThread::createTask: Wrong alignment argument!"
                    << std::endl;
#else
            sif::printError("PosixThread::createTask: "
                    "Wrong alignment argument!\n");
#endif
        }
        return;
    }

    status = pthread_attr_setstack(&attributes, stackPointer, stackSize);
    if(status != 0){
#if FSFW_CPP_OSTREAM_ENABLED == 1
        sif::error << "PosixThread::createTask: pthread_attr_setstack "
                " failed with: " << strerror(status) <<  std::endl;
        sif::error << "Make sure the specified stack size is valid and is "
                "larger than the minimum allowed stack size." << std::endl;
#endif
    }

    status = pthread_attr_setinheritsched(&attributes, PTHREAD_EXPLICIT_SCHED);
    if(status != 0){
#if FSFW_CPP_OSTREAM_ENABLED == 1
        sif::error << "Posix Thread attribute setinheritsched failed with: " <<
                strerror(status) << std::endl;
#endif
    }

    // TODO FIFO -> This needs root privileges for the process
    status = pthread_attr_setschedpolicy(&attributes,SCHED_FIFO);
    if(status != 0){
#if FSFW_CPP_OSTREAM_ENABLED == 1
        sif::error << "Posix Thread attribute schedule policy failed with: " <<
                strerror(status) << std::endl;
#endif
    }

    sched_param scheduleParams;
    scheduleParams.__sched_priority = priority;
    status = pthread_attr_setschedparam(&attributes, &scheduleParams);
    if(status != 0){
#if FSFW_CPP_OSTREAM_ENABLED == 1
        sif::error << "Posix Thread attribute schedule params failed with: " <<
                strerror(status) << std::endl;
#endif
    }

    //Set Signal Mask for suspend until startTask is called
    sigset_t waitSignal;
    sigemptyset(&waitSignal);
    sigaddset(&waitSignal, SIGUSR1);
    status = pthread_sigmask(SIG_BLOCK, &waitSignal, NULL);
    if(status != 0){
#if FSFW_CPP_OSTREAM_ENABLED == 1
        sif::error << "Posix Thread sigmask failed failed with: " <<
                strerror(status) << " errno: " << strerror(errno) << std::endl;
#endif
    }


    status = pthread_create(&thread,&attributes,fnc_,arg_);
    if(status != 0){
#if FSFW_CPP_OSTREAM_ENABLED == 1
        sif::error << "Posix Thread create failed with: " <<
                strerror(status) << std::endl;
#endif
    }

    status = pthread_setname_np(thread,name);
    if(status != 0){
#if FSFW_CPP_OSTREAM_ENABLED == 1
        sif::error << "PosixThread::createTask: setname failed with: " <<
                strerror(status) << std::endl;
#endif
        if(status == ERANGE) {
#if FSFW_CPP_OSTREAM_ENABLED == 1
            sif::error << "PosixThread::createTask: Task name length longer"
                    " than 16 chars. Truncating.." << std::endl;
#endif
            name[15] = '\0';
            status = pthread_setname_np(thread,name);
            if(status != 0){
#if FSFW_CPP_OSTREAM_ENABLED == 1
                sif::error << "PosixThread::createTask: Setting name"
                        " did not work.." << std::endl;
#endif
            }
        }
    }

    status = pthread_attr_destroy(&attributes);
    if(status!=0){
#if FSFW_CPP_OSTREAM_ENABLED == 1
        sif::error << "Posix Thread attribute destroy failed with: " <<
                strerror(status) << std::endl;
#endif
    }
}
