#ifndef FSFW_UNITTEST_CORE_CATCHDEFINITIONS_H_
#define FSFW_UNITTEST_CORE_CATCHDEFINITIONS_H_

#include "fsfw/FSFW.h"
#include <fsfw/ipc/messageQueueDefinitions.h>
#include <fsfw/returnvalues/HasReturnvaluesIF.h>
#include <fsfw/storagemanager/StorageManagerIF.h>

namespace tconst {
static constexpr MessageQueueId_t testQueueId = 42;
}

namespace tglob {
StorageManagerIF* getIpcStoreHandle();
}

#endif /* FSFW_UNITTEST_CORE_CATCHDEFINITIONS_H_ */
