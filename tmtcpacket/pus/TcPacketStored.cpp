#include "../../objectmanager/ObjectManagerIF.h"
#include "../../serviceinterface/ServiceInterfaceStream.h"
#include "TcPacketStored.h"
#include <string.h>

TcPacketStored::TcPacketStored(store_address_t setAddress) :
		TcPacketBase(NULL), storeAddress(setAddress) {
	this->setStoreAddress(this->storeAddress);
}

TcPacketStored::TcPacketStored(uint16_t apid, uint8_t ack, uint8_t service,
		uint8_t subservice, uint8_t sequence_count, const uint8_t* data,
		uint32_t size) :
		TcPacketBase(NULL) {
	this->storeAddress.raw = StorageManagerIF::INVALID_ADDRESS;
	if (!this->checkAndSetStore()) {
		return;
	}
	uint8_t* p_data = NULL;
	ReturnValue_t returnValue = this->store->getFreeElement(&this->storeAddress,
			(TC_PACKET_MIN_SIZE + size), &p_data);
	if (returnValue != this->store->RETURN_OK) {
		return;
	}
	this->setData(p_data);
	initializeTcPacket(apid, sequence_count, ack, service, subservice);
	memcpy(&tcData->data, data, size);
	this->setPacketDataLength(
			size + sizeof(PUSTcDataFieldHeader) + CRC_SIZE - 1);
	this->setErrorControl();
}

TcPacketStored::TcPacketStored() :
		TcPacketBase(NULL) {
	this->storeAddress.raw = StorageManagerIF::INVALID_ADDRESS;
	this->checkAndSetStore();

}

ReturnValue_t TcPacketStored::deletePacket() {
	ReturnValue_t result = this->store->deleteData(this->storeAddress);
	this->storeAddress.raw = StorageManagerIF::INVALID_ADDRESS;
	this->setData( NULL);
	return result;
}

bool TcPacketStored::checkAndSetStore() {
	if (this->store == NULL) {
		this->store = objectManager->get<StorageManagerIF>(objects::TC_STORE);
		if (this->store == NULL) {
			sif::error << "TcPacketStored::TcPacketStored: TC Store not found!"
					<< std::endl;
			return false;
		}
	}
	return true;
}

void TcPacketStored::setStoreAddress(store_address_t setAddress) {
	this->storeAddress = setAddress;
	const uint8_t* temp_data = NULL;
	size_t temp_size;
	ReturnValue_t status = StorageManagerIF::RETURN_FAILED;
	if (this->checkAndSetStore()) {
		status = this->store->getData(this->storeAddress, &temp_data,
				&temp_size);
	}
	if (status == StorageManagerIF::RETURN_OK) {
		this->setData(temp_data);
	} else {
		this->setData(NULL);
		this->storeAddress.raw = StorageManagerIF::INVALID_ADDRESS;
	}
}

store_address_t TcPacketStored::getStoreAddress() {
	return this->storeAddress;
}

bool TcPacketStored::isSizeCorrect() {
	const uint8_t* temp_data = NULL;
	size_t temp_size;
	ReturnValue_t status = this->store->getData(this->storeAddress, &temp_data,
			&temp_size);
	if (status == StorageManagerIF::RETURN_OK) {
		if (this->getFullSize() == temp_size) {
			return true;
		}
	}
	return false;
}

StorageManagerIF* TcPacketStored::store = NULL;

TcPacketStored::TcPacketStored(const uint8_t* data, uint32_t size) :
		TcPacketBase(data) {
	if (getFullSize() != size) {
		return;
	}
	if (this->checkAndSetStore()) {
		ReturnValue_t status = store->addData(&storeAddress, data, size);
		if (status != HasReturnvaluesIF::RETURN_OK) {
			this->setData(NULL);
		}
	}
}
