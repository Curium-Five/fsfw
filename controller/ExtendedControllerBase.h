#ifndef FSFW_CONTROLLER_EXTENDEDCONTROLLERBASE_H_
#define FSFW_CONTROLLER_EXTENDEDCONTROLLERBASE_H_

#include "ControllerBase.h"

#include "../action/HasActionsIF.h"
#include "../datapoollocal/HasLocalDataPoolIF.h"
#include "../action/ActionHelper.h"
#include "../datapoollocal/LocalDataPoolManager.h"

/**
 * @brief   Extendes the basic ControllerBase with the common components
 *          HasActionsIF for commandability and HasLocalDataPoolIF to keep
 *          a pool of local data pool variables.
 * @details
 * Default implementations required for the interfaces will be empty and have
 * to be implemented by child class.
 */
class ExtendedControllerBase: public ControllerBase,
        public HasActionsIF,
        public HasLocalDataPoolIF {
public:
    ExtendedControllerBase(object_id_t objectId, object_id_t parentId,
            size_t commandQueueDepth = 3);

    /** SystemObjectIF overrides */
    virtual ReturnValue_t initialize() override;

    virtual MessageQueueId_t getCommandQueue() const override;

    /** ExecutableObjectIF overrides */
    virtual ReturnValue_t performOperation(uint8_t opCode) override;
    virtual ReturnValue_t initializeAfterTaskCreation() override;

   ProvidesDataPoolSubscriptionIF* getSubscriptionInterface() override;
   AccessPoolManagerIF* getAccessorHandle() override;

protected:
    LocalDataPoolManager localPoolManager;
    ActionHelper actionHelper;

    /**
     * Implemented by child class. Handle all command messages which are
     * not health, mode, action or housekeeping messages.
     * @param message
     * @return
     */
    virtual ReturnValue_t handleCommandMessage(CommandMessage *message) = 0;

    /**
     * Periodic helper from ControllerBase, implemented by child class.
     */
    virtual void performControlOperation() = 0;

    /** Handle the four messages mentioned above */
    void handleQueue() override;

    /** HasActionsIF overrides */
    virtual ReturnValue_t executeAction(ActionId_t actionId,
            MessageQueueId_t commandedBy, const uint8_t* data,
            size_t size) override;

    /** HasLocalDatapoolIF overrides */
    virtual object_id_t getObjectId() const override;
    virtual ReturnValue_t initializeLocalDataPool(
            localpool::DataPool& localDataPoolMap,
            LocalDataPoolManager& poolManager) override;
    virtual uint32_t getPeriodicOperationFrequency() const override;
    virtual LocalPoolDataSetBase* getDataSetHandle(sid_t sid) override;
};



#endif /* FSFW_CONTROLLER_EXTENDEDCONTROLLERBASE_H_ */
