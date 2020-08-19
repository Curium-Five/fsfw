#ifndef FRAMEWORK_FDIR_FAILUREISOLATIONBASE_H_
#define FRAMEWORK_FDIR_FAILUREISOLATIONBASE_H_

#include "../events/EventMessage.h"
#include "ConfirmsFailuresIF.h"
#include "FaultCounter.h"
#include "../health/HealthMessage.h"
#include "../parameters/HasParametersIF.h"
#include "../returnvalues/HasReturnvaluesIF.h"
#include "../ipc/MessageQueueIF.h"

class FailureIsolationBase: public HasReturnvaluesIF,
		public ConfirmsFailuresIF,
		public HasParametersIF {
public:
	static const uint8_t SUBSYSTEM_ID = SUBSYSTEM_ID::FDIR_1;
	static const Event FDIR_CHANGED_STATE = MAKE_EVENT(1, SEVERITY::INFO); //!< FDIR has an internal state, which changed from par2 (oldState) to par1 (newState).
	static const Event FDIR_STARTS_RECOVERY = MAKE_EVENT(2, SEVERITY::MEDIUM); //!< FDIR tries to restart device. Par1: event that caused recovery.
	static const Event FDIR_TURNS_OFF_DEVICE = MAKE_EVENT(3, SEVERITY::MEDIUM); //!< FDIR turns off device. Par1: event that caused recovery.
	FailureIsolationBase(object_id_t owner, object_id_t parent = 0,
			uint8_t messageDepth = 10, uint8_t parameterDomainBase = 0xF0);
	virtual ~FailureIsolationBase();
	virtual ReturnValue_t initialize();
	void checkForFailures();
	MessageQueueId_t getEventReceptionQueue();
	virtual void triggerEvent(Event event, uint32_t parameter1 = 0,
			uint32_t parameter2 = 0);
protected:
	MessageQueueIF* eventQueue;
	object_id_t ownerId;
	HasHealthIF* owner;
	object_id_t faultTreeParent;
	uint8_t parameterDomainBase;
	void setOwnerHealth(HasHealthIF::HealthState health);
	virtual ReturnValue_t eventReceived(EventMessage* event) = 0;
	virtual void eventConfirmed(EventMessage* event);
	virtual void wasParentsFault(EventMessage* event);
	virtual ReturnValue_t confirmFault(EventMessage* event);
	virtual void decrementFaultCounters() = 0;
	ReturnValue_t sendConfirmationRequest(EventMessage* event,
			MessageQueueId_t destination = 0);
	void throwFdirEvent(Event event, uint32_t parameter1 = 0,
			uint32_t parameter2 = 0);
private:
	void doConfirmFault(EventMessage* event);bool isFdirDisabledForSeverity(
			EventSeverity_t severity);
};

#endif /* FRAMEWORK_FDIR_FAILUREISOLATIONBASE_H_ */
