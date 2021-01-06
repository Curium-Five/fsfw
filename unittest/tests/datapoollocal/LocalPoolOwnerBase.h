#ifndef FSFW_UNITTEST_TESTS_DATAPOOLLOCAL_LOCALPOOLOWNERBASE_H_
#define FSFW_UNITTEST_TESTS_DATAPOOLLOCAL_LOCALPOOLOWNERBASE_H_

#include <fsfw/datapoollocal/HasLocalDataPoolIF.h>
#include <fsfw/objectmanager/SystemObject.h>
#include <fsfw/datapoollocal/LocalPoolVariable.h>
#include <fsfw/datapoollocal/LocalPoolVector.h>
#include <fsfw/ipc/QueueFactory.h>
#include <testcfg/objects/systemObjectList.h>

namespace lpool {
static constexpr lp_id_t uint8VarId = 0;
static constexpr lp_id_t floatVarId = 1;
static constexpr lp_id_t uint32VarId = 2;
static constexpr lp_id_t uint16Vec3Id = 3;
static constexpr lp_id_t int64Vec2Id = 4;
}



class LocalPoolOwnerBase: public SystemObject, public HasLocalDataPoolIF {
public:
	LocalPoolOwnerBase(
			object_id_t objectId = objects::TEST_LOCAL_POOL_OWNER_BASE):
		SystemObject(objectId), hkManager(this, messageQueue)  {
		messageQueue = QueueFactory::instance()->createMessageQueue(10);
	}

	~LocalPoolOwnerBase() {
		QueueFactory::instance()->deleteMessageQueue(messageQueue);
	}

	object_id_t getObjectId() const override {
		return SystemObject::getObjectId();
	}

	ReturnValue_t initializeHkManager() {
		if(not initialized) {
			initialized = true;
			return hkManager.initialize(messageQueue);
		}
		return HasReturnvaluesIF::RETURN_OK;
	}

	ReturnValue_t initializeHkManagerAfterTaskCreation() {
		if(not initializedAfterTaskCreation) {
			initializedAfterTaskCreation = true;
			return hkManager.initializeAfterTaskCreation();
		}
		return HasReturnvaluesIF::RETURN_OK;
	}

	/** Command queue for housekeeping messages. */
	MessageQueueId_t getCommandQueue() const override {
		return messageQueue->getId();
	}

	// This is called by initializeAfterTaskCreation of the HK manager.
	virtual ReturnValue_t initializeLocalDataPool(
	        LocalDataPool& localDataPoolMap,
	        LocalDataPoolManager& poolManager) {
		// Default initialization empty for now.
		localDataPoolMap.emplace(lpool::uint8VarId,
				new PoolEntry<uint8_t>({0}));
		localDataPoolMap.emplace(lpool::floatVarId,
				new PoolEntry<float>({0}));
		localDataPoolMap.emplace(lpool::uint32VarId,
				new PoolEntry<uint32_t>({0}));

		localDataPoolMap.emplace(lpool::uint16Vec3Id,
				new PoolEntry<uint16_t>({0, 0, 0}));
		localDataPoolMap.emplace(lpool::int64Vec2Id,
				new PoolEntry<int64_t>({0, 0}));
		return HasReturnvaluesIF::RETURN_OK;
	}

	LocalDataPoolManager* getHkManagerHandle() override {
		return &hkManager;
	}

	uint32_t getPeriodicOperationFrequency() const override {
		return 0;
	}

	/**
	 * This function is used by the pool manager to get a valid dataset
     * from a SID
	 * @param sid Corresponding structure ID
	 * @return
	 */
	virtual LocalPoolDataSetBase* getDataSetHandle(sid_t sid) override  {
		// empty for now
		return nullptr;
	}
private:

	lp_var_t<uint8_t> testUint8 = lp_var_t<uint8_t>(this, lpool::uint8VarId);
	lp_var_t<float> testFloat = lp_var_t<float>(this, lpool::floatVarId);
	lp_var_t<uint32_t> testUint32 = lp_var_t<uint32_t>(this, lpool::uint32VarId);

	lp_vec_t<uint16_t, 3> testUint16Vec = lp_vec_t<uint16_t, 3>(this,
			lpool::uint16Vec3Id);
	lp_vec_t<int64_t, 2> testInt64Vec = lp_vec_t<int64_t, 2>(this,
			lpool::int64Vec2Id);

	MessageQueueIF* messageQueue = nullptr;
	LocalDataPoolManager hkManager;

	bool initialized = false;
	bool initializedAfterTaskCreation = false;

};

#endif /* FSFW_UNITTEST_TESTS_DATAPOOLLOCAL_LOCALPOOLOWNERBASE_H_ */
