#ifndef FSFW_DATAPOOLLOCAL_HASLOCALDATAPOOLIF_H_
#define FSFW_DATAPOOLLOCAL_HASLOCALDATAPOOLIF_H_

#include "localPoolDefinitions.h"

#include "../datapool/PoolEntryIF.h"
#include "../serviceinterface/ServiceInterface.h"
#include "../ipc/MessageQueueSenderIF.h"
#include "../housekeeping/HousekeepingMessage.h"

#include <map>

class AccessPoolManagerIF;
class ProvidesDataPoolSubscriptionIF;
class LocalPoolDataSetBase;
class LocalPoolObjectBase;
class LocalDataPoolManager;

/**
 * @brief 		This interface is implemented by classes which posses a local data pool (not the
 * 				managing class). It defines the relationship between the local data pool owner
 * 				and the LocalDataPoolManager.
 * @details
 * Any class implementing this interface shall also have a LocalDataPoolManager member class which
 * contains the actual pool data structure and exposes the public interface for it.
 * The local data pool can be accessed using helper classes by using the
 * LocalPoolVariable, LocalPoolVector or LocalDataSet classes. Every local pool variable can
 * be uniquely identified by a global pool ID (gp_id_t) and every dataset tied
 * to a pool manager can be uniqely identified by a global structure ID (sid_t).
 *
 */
class HasLocalDataPoolIF {
    friend class HasLocalDpIFManagerAttorney;
    friend class HasLocalDpIFUserAttorney;
public:
    virtual~ HasLocalDataPoolIF() {};

    static constexpr uint32_t INVALID_LPID = localpool::INVALID_LPID;

    virtual object_id_t getObjectId() const = 0;

    /** Command queue for housekeeping messages. */
    virtual MessageQueueId_t getCommandQueue() const = 0;

    /**
     * Is used by pool owner to initialize the pool map once
     * The manager instance shall also be passed to this function.
     * It can be used to subscribe for periodic packets for for updates.
     */
    virtual ReturnValue_t initializeLocalDataPool(localpool::DataPool& localDataPoolMap,
            LocalDataPoolManager& poolManager) = 0;

    /**
     * Returns the minimum sampling frequency in milliseconds, which will
     * usually be the period the pool owner performs its periodic operation.
     * @return
     */
    virtual uint32_t getPeriodicOperationFrequency() const = 0;

    /**
     * @brief   This function will be called by the manager if an update
     *          notification is received.
     * @details  HasLocalDataPoolIF
     * Can be overriden by the child class to handle changed datasets.
     * @param sid
     * @param storeId If a snapshot was requested, data will be located inside
     * the IPC store with this store ID.
     */
    virtual void handleChangedDataset(sid_t sid,
            store_address_t storeId = storeId::INVALID_STORE_ADDRESS) {
        return;
    }

    /**
     * @brief   This function will be called by the manager if an update
     *          notification is received.
     * @details
     * Can be overriden by the child class to handle changed pool IDs.
     * @param sid
     * @param storeId If a snapshot was requested, data will be located inside
     * the IPC store with this store ID.
     */
    virtual void handleChangedPoolVariable(lp_id_t poolId,
            store_address_t storeId = storeId::INVALID_STORE_ADDRESS) {
        return;
    }

    /**
     * These function can be implemented by pool owner, if they are required
     * and used by the housekeeping message interface.
     * */
    virtual ReturnValue_t addDataSet(sid_t sid) {
        return HasReturnvaluesIF::RETURN_FAILED;
    };
    virtual ReturnValue_t removeDataSet(sid_t sid) {
        return HasReturnvaluesIF::RETURN_FAILED;
    };
    virtual ReturnValue_t changeCollectionInterval(sid_t sid, float newIntervalSeconds) {
        return HasReturnvaluesIF::RETURN_FAILED;
    };

    /**
     * This function can be used by data pool consumers to retrieve a handle
     * which allows subscriptions to dataset and variable updates in form of messages.
     * The consumers can then read the most recent variable value by calling read with
     * an own pool variable or set instance or using the deserialized snapshot data.
     * @return
     */
    virtual ProvidesDataPoolSubscriptionIF* getSubscriptionInterface() = 0;

protected:

    /**
     * Accessor handle required for internal handling. Not intended for users and therefore
     * declared protected. Users should instead use pool variables, sets or the subscription
     * interface to access pool entries.
     * @return
     */
    virtual AccessPoolManagerIF* getAccessorHandle() = 0;

    /**
     * This function is used by the pool manager to get a valid dataset
     * from a SID. This function is protected to prevent users from
     * using raw data set pointers which could not be thread-safe. Users
     * should use the #ProvidesDataPoolSubscriptionIF.
     * @param sid Corresponding structure ID
     * @return
     */
    virtual LocalPoolDataSetBase* getDataSetHandle(sid_t sid) = 0;

    /**
     * Similar to the function above, but used to get a local pool variable
     * handle. This is only needed for update notifications, so it is not
     * defined as abstract. This function is protected to prevent users from
     * using raw pool variable pointers which could not be thread-safe.
     * Users should use the #ProvidesDataPoolSubscriptionIF.
     * @param localPoolId
     * @return
     */
    virtual LocalPoolObjectBase* getPoolObjectHandle(lp_id_t localPoolId) {
#if FSFW_CPP_OSTREAM_ENABLED == 1
        sif::warning << "HasLocalDataPoolIF::getPoolObjectHandle: Not overriden"
                << ". Returning nullptr!" << std::endl;
#else
        sif::printWarning("HasLocalDataPoolIF::getPoolObjectHandle: "
                "Not overriden. Returning nullptr!\n");
#endif
        return nullptr;
    }
};

#endif /* FSFW_DATAPOOLLOCAL_HASLOCALDATAPOOLIF_H_ */
