#include <framework/ipc/MutexFactory.h>
#include <framework/ipc/MutexHelper.h>
#include <framework/osal/host/QueueMapManager.h>

QueueMapManager* QueueMapManager::mqManagerInstance = nullptr;

QueueMapManager::QueueMapManager() {}

QueueMapManager* QueueMapManager::instance() {
	if (mqManagerInstance == nullptr){
		mqManagerInstance = new QueueMapManager();
	}
	return QueueMapManager::mqManagerInstance;
}

ReturnValue_t QueueMapManager::addMessageQueue(
		MessageQueueIF* queueToInsert, MessageQueueId_t* id) {
	uint32_t currentId = queueCounter++;
	auto returnPair = queueMap.emplace(currentId, queueToInsert);
	if(not returnPair.second) {
		// this should never happen for the atomic variable.
		sif::error << "QueueMapManager: This ID is already inside the map!"
				<< std::endl;
		return HasReturnvaluesIF::RETURN_FAILED;
	}
	if (id != nullptr) {
		*id = currentId;
	}
	mapLock = MutexFactory::instance()->createMutex();
	return HasReturnvaluesIF::RETURN_OK;
}

MessageQueueIF* QueueMapManager::getMessageQueue(
		MessageQueueId_t messageQueueId) const {
	MutexHelper(mapLock, 50);
	auto queueIter = queueMap.find(messageQueueId);
	if(queueIter != queueMap.end()) {
		return queueIter->second;
	}
	else {
		sif::warning << "QueueMapManager::getQueueHandle: The ID" <<
				messageQueueId << " does not exists in the map" << std::endl;
		return nullptr;
	}
}

