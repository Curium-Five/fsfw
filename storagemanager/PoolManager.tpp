#ifndef FRAMEWORK_STORAGEMANAGER_POOLMANAGER_TPP_
#define FRAMEWORK_STORAGEMANAGER_POOLMANAGER_TPP_

template<uint8_t NUMBER_OF_POOLS>
inline PoolManager<NUMBER_OF_POOLS>::PoolManager(object_id_t setObjectId,
		const uint16_t element_sizes[NUMBER_OF_POOLS],
		const uint16_t n_elements[NUMBER_OF_POOLS]) :
		LocalPool<NUMBER_OF_POOLS>(setObjectId, element_sizes, n_elements, true) {
	mutex = MutexFactory::instance()->createMutex();
}

template<uint8_t NUMBER_OF_POOLS>
inline PoolManager<NUMBER_OF_POOLS>::~PoolManager(void) {
	MutexFactory::instance()->deleteMutex(mutex);
}

template<uint8_t NUMBER_OF_POOLS>
inline ReturnValue_t PoolManager<NUMBER_OF_POOLS>::reserveSpace(
		const uint32_t size, store_address_t* address, bool ignoreFault) {
	MutexHelper mutexHelper(mutex,MutexIF::NO_TIMEOUT);
	ReturnValue_t status = LocalPool<NUMBER_OF_POOLS>::reserveSpace(size,
			address,ignoreFault);
	return status;
}

template<uint8_t NUMBER_OF_POOLS>
inline ReturnValue_t PoolManager<NUMBER_OF_POOLS>::deleteData(
		store_address_t packet_id) {
	// debug << "PoolManager( " << translateObject(getObjectId()) <<
	//       " )::deleteData from store " << packet_id.pool_index <<
	//       ". id is "<< packet_id.packet_index << std::endl;
	MutexHelper mutexHelper(mutex,MutexIF::NO_TIMEOUT);
	ReturnValue_t status = LocalPool<NUMBER_OF_POOLS>::deleteData(packet_id);
	return status;
}

template<uint8_t NUMBER_OF_POOLS>
inline ReturnValue_t PoolManager<NUMBER_OF_POOLS>::deleteData(uint8_t* buffer,
		size_t size, store_address_t* storeId) {
	MutexHelper mutexHelper(mutex,MutexIF::NO_TIMEOUT);
	ReturnValue_t status = LocalPool<NUMBER_OF_POOLS>::deleteData(buffer,
			size, storeId);
	return status;
}

template<uint8_t NUMBER_OF_POOLS>
inline ReturnValue_t PoolManager<NUMBER_OF_POOLS>::modifyData(
		store_address_t packet_id, uint8_t** packet_ptr, size_t* size) {
	MutexHelper mutexHelper(mutex,MutexIF::NO_TIMEOUT);
	ReturnValue_t status = LocalPool<NUMBER_OF_POOLS>::modifyData(packet_id,
				packet_ptr, size);
	return status;
}

#endif
