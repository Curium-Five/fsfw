#ifndef FSFW_STORAGEMANAGER_LOCALPOOL_H_
#define FSFW_STORAGEMANAGER_LOCALPOOL_H_

#include "StorageManagerIF.h"
#include "../objectmanager/SystemObject.h"
#include "../objectmanager/ObjectManagerIF.h"
#include "../serviceinterface/ServiceInterfaceStream.h"
#include "../internalError/InternalErrorReporterIF.h"
#include "../storagemanager/StorageAccessor.h"

#include <vector>
#include <set>
#include <utility>
#include <limits>

/**
 * @brief   The LocalPool class provides an intermediate data storage with
 *          a fixed pool size policy.
 * @details The class implements the StorageManagerIF interface. While the
 *          total number of pools is fixed, the element sizes in one pool and
 *          the number of pool elements per pool are set on construction.
 *          The full amount of memory is allocated on construction.
 *          The overhead is 4 byte per pool element to store the size
 *          information of each stored element.
 *          To maintain an "empty" information, the pool size is limited to
 *          0xFFFF-1 bytes.
 *          It is possible to store empty packets in the pool.
 *          The local pool is NOT thread-safe.
 */
class LocalPool: public SystemObject, public StorageManagerIF {
public:
    using size_type = size_t;

    using poolElementSize = size_type;
    using numberPoolElements = uint16_t;
    using LocalPoolCfgPair = std::pair<poolElementSize, numberPoolElements>;
    using LocalPoolConfig = std::multiset<LocalPoolCfgPair>;
    /**
     * @brief   This definition generally sets the number of different sized pools.
     * @details This must be less than the maximum number of pools (currently 0xff).
     */
    const uint8_t NUMBER_OF_POOLS;
    /**
     * @brief   This is the default constructor for a pool manager instance.
     * @details By passing two arrays of size NUMBER_OF_POOLS, the constructor
     *          allocates memory (with @c new) for store and size_list. These
     *          regions are all set to zero on start up.
     * @param setObjectId   The object identifier to be set. This allows for
     *                      multiple instances of LocalPool in the system.
     * @param element_sizes An array of size NUMBER_OF_POOLS in which the size
     *                      of a single element in each pool is determined.
     *                      <b>The sizes must be provided in ascending order.
     *                      </b>
     * @param n_elements    An array of size NUMBER_OF_POOLS in which the
     *                      number of elements for each pool is determined.
     *                      The position of these values correspond to those in
     *                      element_sizes.
     * @param registered    Register the pool in object manager or not.
     * Default is false (local pool).
     * @param spillsToHigherPools A variable to determine whether
     * higher n pools are used if the store is full.
     */
    LocalPool(object_id_t setObjectId, const LocalPoolConfig poolConfig,
            bool registered = false, bool spillsToHigherPools = false);
	/**
	 * @brief	In the LocalPool's destructor all allocated memory is freed.
	 */
	virtual ~LocalPool(void);

	/**
	 * Documentation: See StorageManagerIF.h
	 */
	ReturnValue_t addData(store_address_t* storeId, const uint8_t * data,
			size_t size, bool ignoreFault = false) override;
	ReturnValue_t getFreeElement(store_address_t* storeId,const size_t size,
			uint8_t** pData, bool ignoreFault = false) override;

	ConstAccessorPair getData(store_address_t storeId) override;
	ReturnValue_t getData(store_address_t storeId,
	        ConstStorageAccessor& constAccessor) override;
	ReturnValue_t getData(store_address_t storeId,
	        const uint8_t** packet_ptr, size_t * size) override;

	AccessorPair modifyData(store_address_t storeId) override;
	ReturnValue_t modifyData(store_address_t storeId,
	        StorageAccessor& storeAccessor) override;
	ReturnValue_t modifyData(store_address_t storeId, uint8_t** packet_ptr,
			size_t * size) override;

	virtual ReturnValue_t deleteData(store_address_t storeId) override;
	virtual ReturnValue_t deleteData(uint8_t* ptr, size_t size,
			store_address_t* storeId = nullptr) override;
	void clearStore() override;
	ReturnValue_t initialize() override;
protected:
	/**
	 * With this helper method, a free element of @c size is reserved.
	 * @param size	The minimum packet size that shall be reserved.
	 * @param[out] address Storage ID of the reserved data.
	 * @return	- #RETURN_OK on success,
	 * 			- the return codes of #getPoolIndex or #findEmpty otherwise.
	 */
	virtual ReturnValue_t reserveSpace(const size_t size,
			store_address_t* address, bool ignoreFault);

private:
    /**
     * Indicates that this element is free.
     * This value limits the maximum size of a pool.
     * Change to larger data type if increase is required.
     */
    static const size_type STORAGE_FREE = std::numeric_limits<size_type>::max();
	/**
	 * @brief	In this array, the element sizes of each pool is stored.
	 * @details	The sizes are maintained for internal pool management. The sizes
	 * 			must be set in ascending order on construction.
	 */
    std::vector<size_type> elementSizes =
            std::vector<size_type>(NUMBER_OF_POOLS);
	/**
	 * @brief	n_elements stores the number of elements per pool.
	 * @details	These numbers are maintained for internal pool management.
	 */
	std::vector<uint16_t> numberOfElements =
	        std::vector<uint16_t>(NUMBER_OF_POOLS);
	/**
	 * @brief	store represents the actual memory pool.
	 * @details	It is an array of pointers to memory, which was allocated with
	 * 			a @c new call on construction.
	 */
	std::vector<std::vector<uint8_t>> store =
	        std::vector<std::vector<uint8_t>>(NUMBER_OF_POOLS);

	/**
	 * @brief	The size_list attribute stores the size values of every pool element.
	 * @details	As the number of elements is determined on construction, the size list
	 * 			is also dynamically allocated there.
	 */
	std::vector<std::vector<size_type>> sizeLists =
	        std::vector<std::vector<size_type>>(NUMBER_OF_POOLS);

	//! A variable to determine whether higher n pools are used if
	//! the store is full.
	bool spillsToHigherPools = false;
	/**
	 * @brief	This method safely stores the given data in the given packet_id.
	 * @details	It also sets the size in size_list. The method does not perform
	 * 			any range checks, these are done in advance.
	 * @param packet_id	The storage identifier in which the data shall be stored.
	 * @param data		The data to be stored.
	 * @param size		The size of the data to be stored.
	 */
	void write(store_address_t packetId, const uint8_t* data, size_t size);
	/**
	 * @brief	A helper method to read the element size of a certain pool.
	 * @param pool_index	The pool in which to look.
	 * @return	Returns the size of an element or 0.
	 */
	size_type getPageSize(uint16_t poolIndex);
	/**
	 * @brief	This helper method looks up a fitting pool for a given size.
	 * @details	The pools are looked up in ascending order, so the first that
	 * 			fits is used.
	 * @param packet_size	The size of the data to be stored.
	 * @return	Returns the pool that fits or StorageManagerIF::INVALID_ADDRESS.
	 */
	/**
	 * @brief	This helper method looks up a fitting pool for a given size.
	 * @details	The pools are looked up in ascending order, so the first that
	 * 			fits is used.
	 * @param packet_size		The size of the data to be stored.
	 * @param[out] poolIndex	The fitting pool index found.
	 * @return	- #RETURN_OK on success,
	 * 			- #DATA_TOO_LARGE otherwise.
	 */
	ReturnValue_t getPoolIndex(size_t packetSize, uint16_t* poolIndex,
			uint16_t startAtIndex = 0);
	/**
	 * @brief	This helper method calculates the true array position in store
	 * 			of a given packet id.
	 * @details	The method does not perform any range checks, these are done in
	 * 			advance.
	 * @param packet_id	The packet id to look up.
	 * @return	Returns the position of the data in store.
	 */
	size_type getRawPosition(store_address_t storeId);
	/**
	 * @brief	This is a helper method to find an empty element in a given pool.
	 * @details	The method searches size_list for the first empty element, so
	 * 			duration grows with the fill level of the pool.
	 * @param pool_index	The pool in which the search is performed.
	 * @param[out] element	The first found element in the pool.
	 * @return	- #RETURN_OK on success,
	 * 			- #DATA_STORAGE_FULL if the store is full
	 */
	ReturnValue_t findEmpty(uint16_t poolIndex, uint16_t* element);

    InternalErrorReporterIF *internalErrorReporter = nullptr;
};

#endif /* FSFW_STORAGEMANAGER_LOCALPOOL_H_ */
