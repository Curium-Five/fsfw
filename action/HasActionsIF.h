#ifndef HASACTIONSIF_H_
#define HASACTIONSIF_H_

#include <framework/action/ActionHelper.h>
#include <framework/action/ActionMessage.h>
#include <framework/action/SimpleActionHelper.h>
#include <framework/returnvalues/HasReturnvaluesIF.h>
#include <framework/ipc/MessageQueueIF.h>
/**
 * @brief
 * Interface for component which uses actions
 *
 * @details
 * This interface is used to execute actions in the component. Actions, in the sense of this interface, are activities with a well-defined beginning and
 * end in time. They may adjust sub-states of components, but are not supposed to change
 * the main mode of operation, which is handled with the HasModesIF described below.
 *
 * The HasActionsIF allows components to define such actions and make them available
 * for other components to use. Implementing the interface is straightforward: There’s a
 * single executeAction call, which provides an identifier for the action to execute, as well
 * as arbitrary parameters for input. Aside from direct, software-based
 * actions, it is used in device handler components as an interface to forward commands to
 * devices.
 * Implementing components of the interface are supposed to check identifier (ID) and
 * parameters and immediately start execution of the action. It is, however, not required to
 * immediately finish execution. Instead, this may be deferred to a later point in time, at
 * which the component needs to inform the caller about finished or failed execution.
 *
 * \ingroup interfaces
 */
class HasActionsIF {
public:
	static const uint8_t INTERFACE_ID = CLASS_ID::HAS_ACTIONS_IF;
	static const ReturnValue_t IS_BUSY = MAKE_RETURN_CODE(1);//!<
	static const ReturnValue_t INVALID_PARAMETERS = MAKE_RETURN_CODE(2);
	static const ReturnValue_t EXECUTION_FINISHED = MAKE_RETURN_CODE(3);
	static const ReturnValue_t INVALID_ACTION_ID = MAKE_RETURN_CODE(4);
	virtual ~HasActionsIF() { }
	/**
	 * Function to get the MessageQueueId_t of the implementing object
	 * @return MessageQueueId_t of the object
	 */
	virtual MessageQueueId_t getCommandQueue() const = 0;
	/**
	 * Execute or initialize the execution of a certain function.
	 * Returning #EXECUTION_FINISHED or a failure code, nothing else needs to be done.
	 * When needing more steps, return RETURN_OK and issue steps and completion manually. One "step failed" or completion report must
	 * be issued!
	 */
	virtual ReturnValue_t executeAction(ActionId_t actionId, MessageQueueId_t commandedBy, const uint8_t* data, uint32_t size) = 0;
};


#endif /* HASACTIONSIF_H_ */
