#ifndef FSFW_DATAPOOLLOCAL_POOLREADHELPER_H_
#define FSFW_DATAPOOLLOCAL_POOLREADHELPER_H_

#include "LocalPoolDataSetBase.h"
#include "../serviceinterface/ServiceInterface.h"
#include <FSFWConfig.h>

/**
 * @brief 	Helper class to read data sets or pool variables
 */
class PoolReadHelper {
public:
	PoolReadHelper(ReadCommitIF* readObject,
			MutexIF::TimeoutType timeoutType = MutexIF::TimeoutType::WAITING,
			uint32_t mutexTimeout = 20):
			readObject(readObject), mutexTimeout(mutexTimeout) {
		if(readObject != nullptr) {
			readResult = readObject->read(timeoutType, mutexTimeout);
#if FSFW_PRINT_VERBOSITY_LEVEL == 1
#if FSFW_CPP_OSTREAM_ENABLED == 1
			sif::error << "PoolReadHelper: Read failed!" << std::endl;
#endif
#else
			sif::printError("PoolReadHelper: Read failed!\n");
#endif
		}
	}

	ReturnValue_t getReadResult() const {
		return readResult;
	}

	~PoolReadHelper() {
		if(readObject != nullptr) {
			readObject->commit(timeoutType, mutexTimeout);
		}

	}

private:
	ReadCommitIF* readObject = nullptr;
	ReturnValue_t readResult = HasReturnvaluesIF::RETURN_OK;
	MutexIF::TimeoutType timeoutType = MutexIF::TimeoutType::WAITING;
	uint32_t mutexTimeout = 20;
};



#endif /* FSFW_DATAPOOLLOCAL_POOLREADHELPER_H_ */
