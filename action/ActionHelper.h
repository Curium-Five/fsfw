#ifndef ACTIONHELPER_H_
#define ACTIONHELPER_H_

#include <framework/action/ActionMessage.h>
#include <framework/serialize/SerializeIF.h>
#include <framework/ipc/MessageQueueIF.h>

class HasActionsIF;

class ActionHelper {
public:
	ActionHelper(HasActionsIF* setOwner, MessageQueueIF* useThisQueue);
	virtual ~ActionHelper();
	ReturnValue_t handleActionMessage(CommandMessage* command);
	ReturnValue_t initialize();
	void step(uint8_t step, MessageQueueId_t reportTo, ActionId_t commandId, ReturnValue_t result = HasReturnvaluesIF::RETURN_OK);
	void finish(MessageQueueId_t reportTo, ActionId_t commandId, ReturnValue_t result = HasReturnvaluesIF::RETURN_OK);
	ReturnValue_t reportData(MessageQueueId_t reportTo, ActionId_t replyId, SerializeIF* data, bool hideSender = false);
	void setQueueToUse(MessageQueueIF *queue);
protected:
	static const uint8_t STEP_OFFSET = 1;
	HasActionsIF* owner;
	MessageQueueIF* queueToUse;
	StorageManagerIF* ipcStore;
	virtual void prepareExecution(MessageQueueId_t commandedBy, ActionId_t actionId, store_address_t dataAddress);
	void resetHelper();
};

#endif /* ACTIONHELPER_H_ */
