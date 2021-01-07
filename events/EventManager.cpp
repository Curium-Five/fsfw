#include "EventManager.h"
#include "EventMessage.h"

#include <FSFWConfig.h>
#include "../serviceinterface/ServiceInterfaceStream.h"
#include "../ipc/QueueFactory.h"
#include "../ipc/MutexFactory.h"


// If one checks registerListener calls, there are around 40 (to max 50)
// objects registering for certain events.
// Each listener requires 1 or 2 EventIdMatcher and 1 or 2 ReportRangeMatcher.
// So a good guess is 75 to a max of 100 pools required for each, which fits well.
const LocalPool::LocalPoolConfig EventManager::poolConfig = {
        {fsfwconfig::FSFW_EVENTMGMR_MATCHTREE_NODES,
        		sizeof(EventMatchTree::Node)},
        {fsfwconfig::FSFW_EVENTMGMT_EVENTIDMATCHERS,
        		sizeof(EventIdRangeMatcher)},
        {fsfwconfig::FSFW_EVENTMGMR_RANGEMATCHERS,
        		sizeof(ReporterRangeMatcher)}
};

EventManager::EventManager(object_id_t setObjectId) :
		SystemObject(setObjectId),
		factoryBackend(0, poolConfig, false, true) {
	mutex = MutexFactory::instance()->createMutex();
	eventReportQueue = QueueFactory::instance()->createMessageQueue(
			MAX_EVENTS_PER_CYCLE, EventMessage::EVENT_MESSAGE_SIZE);
}

EventManager::~EventManager() {
	QueueFactory::instance()->deleteMessageQueue(eventReportQueue);
	MutexFactory::instance()->deleteMutex(mutex);
}

MessageQueueId_t EventManager::getEventReportQueue() {
	return eventReportQueue->getId();
}

ReturnValue_t EventManager::performOperation(uint8_t opCode) {
	ReturnValue_t result = HasReturnvaluesIF::RETURN_OK;
	while (result == HasReturnvaluesIF::RETURN_OK) {
		EventMessage message;
		result = eventReportQueue->receiveMessage(&message);
		if (result == HasReturnvaluesIF::RETURN_OK) {
#if FSFW_OBJ_EVENT_TRANSLATION == 1
			printEvent(&message);
#endif
			notifyListeners(&message);
		}
	}
	return HasReturnvaluesIF::RETURN_OK;
}

void EventManager::notifyListeners(EventMessage* message) {
	lockMutex();
	for (auto iter = listenerList.begin(); iter != listenerList.end(); ++iter) {
		if (iter->second.match(message)) {
			MessageQueueSenderIF::sendMessage(iter->first, message,
					message->getSender());
		}
	}
	unlockMutex();
}

ReturnValue_t EventManager::registerListener(MessageQueueId_t listener,
bool forwardAllButSelected) {
	auto result = listenerList.insert(
			std::pair<MessageQueueId_t, EventMatchTree>(listener,
					EventMatchTree(&factoryBackend, forwardAllButSelected)));
	if (!result.second) {
		return HasReturnvaluesIF::RETURN_FAILED;
	}
	return HasReturnvaluesIF::RETURN_OK;
}

ReturnValue_t EventManager::subscribeToEvent(MessageQueueId_t listener,
		EventId_t event) {
	return subscribeToEventRange(listener, event);
}

ReturnValue_t EventManager::subscribeToAllEventsFrom(MessageQueueId_t listener,
		object_id_t object) {
	return subscribeToEventRange(listener, 0, 0, true, object);
}

ReturnValue_t EventManager::subscribeToEventRange(MessageQueueId_t listener,
		EventId_t idFrom, EventId_t idTo, bool idInverted,
		object_id_t reporterFrom, object_id_t reporterTo,
		bool reporterInverted) {
	auto iter = listenerList.find(listener);
	if (iter == listenerList.end()) {
		return LISTENER_NOT_FOUND;
	}
	lockMutex();
	ReturnValue_t result = iter->second.addMatch(idFrom, idTo, idInverted,
			reporterFrom, reporterTo, reporterInverted);
	unlockMutex();
	return result;
}

ReturnValue_t EventManager::unsubscribeFromEventRange(MessageQueueId_t listener,
		EventId_t idFrom, EventId_t idTo, bool idInverted,
		object_id_t reporterFrom, object_id_t reporterTo,
		bool reporterInverted) {
	auto iter = listenerList.find(listener);
	if (iter == listenerList.end()) {
		return LISTENER_NOT_FOUND;
	}
	lockMutex();
	ReturnValue_t result = iter->second.removeMatch(idFrom, idTo, idInverted,
			reporterFrom, reporterTo, reporterInverted);
	unlockMutex();
	return result;
}

#if FSFW_OBJ_EVENT_TRANSLATION == 1

void EventManager::printEvent(EventMessage* message) {
	const char *string = 0;
	switch (message->getSeverity()) {
	case severity::INFO:
#if DEBUG_INFO_EVENT == 1
		string = translateObject(message->getReporter());
#if FSFW_CPP_OSTREAM_ENABLED == 1
		sif::info << "EVENT: ";
		if (string != 0) {
			sif::info << string;
		} else {
			sif::info << "0x" << std::hex << message->getReporter() << std::dec;
		}
		sif::info << " reported " << translateEvents(message->getEvent()) << " ("
				<< std::dec << message->getEventId() << std::hex << ") P1: 0x"
				<< message->getParameter1() << " P2: 0x"
				<< message->getParameter2() << std::dec << std::endl;
#endif /* FSFW_CPP_OSTREAM_ENABLED == 1 */
#endif /* DEBUG_INFO_EVENT == 1 */
		break;
	default:
		string = translateObject(message->getReporter());
#if FSFW_CPP_OSTREAM_ENABLED == 1
		sif::debug << "EventManager: ";
		if (string != 0) {
			sif::debug << string;
		}
		else {
			sif::debug << "0x" << std::hex << message->getReporter() << std::dec;
		}
		sif::debug << " reported " << translateEvents(message->getEvent())
				<< " (" << std::dec << message->getEventId() << ") "
				<< std::endl;
		sif::debug << std::hex << "P1 Hex: 0x" << message->getParameter1()
				<< ", P1 Dec: " << std::dec << message->getParameter1()
				<< std::endl;
		sif::debug << std::hex << "P2 Hex: 0x" << message->getParameter2()
				<< ", P2 Dec: " <<  std::dec << message->getParameter2()
				<< std::endl;
#endif
		break;
	}
}
#endif

void EventManager::lockMutex() {
	mutex->lockMutex(timeoutType, timeoutMs);
}

void EventManager::unlockMutex() {
	mutex->unlockMutex();
}

void EventManager::setMutexTimeout(MutexIF::TimeoutType timeoutType,
		uint32_t timeoutMs) {
	this->timeoutType = timeoutType;
	this->timeoutMs = timeoutMs;
}
