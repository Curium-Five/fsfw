#include "fsfw/tmtcpacket/cfdp/CFDPPacketStored.h"

#include "fsfw/objectmanager/ObjectManager.h"

CFDPPacketStored::CFDPPacketStored() : CFDPPacket(nullptr) {}

CFDPPacketStored::CFDPPacketStored(store_address_t setAddress) : CFDPPacket(nullptr) {
  this->setStoreAddress(setAddress);
}

CFDPPacketStored::CFDPPacketStored(const uint8_t* data, size_t size) : CFDPPacket(data) {
  if (this->getFullSize() != size) {
    return;
  }
  if (this->checkAndSetStore()) {
    ReturnValue_t status = STORE->addData(&storeAddress, data, size);
    if (status != HasReturnvaluesIF::RETURN_OK) {
      this->setData(nullptr, -1, nullptr);
    }
    const uint8_t* storePtr = nullptr;
    // Repoint base data pointer to the data in the store.
    STORE->getData(storeAddress, &storePtr, &size);
    this->setData(const_cast<uint8_t*>(storePtr), size, nullptr);
  }
}

ReturnValue_t CFDPPacketStored::deletePacket() {
  ReturnValue_t result = STORE->deleteData(this->storeAddress);
  this->storeAddress.raw = StorageManagerIF::INVALID_ADDRESS;
  // To circumvent size checks
  this->setData(nullptr, -1, nullptr);
  return result;
}

// CFDPPacket* CFDPPacketStored::getPacketBase() {
//     return this;
// }
void CFDPPacketStored::setStoreAddress(store_address_t setAddress) {
  this->storeAddress = setAddress;
  const uint8_t* tempData = nullptr;
  size_t tempSize;
  ReturnValue_t status = StorageManagerIF::RETURN_FAILED;
  if (this->checkAndSetStore()) {
    status = STORE->getData(this->storeAddress, &tempData, &tempSize);
  }
  if (status == StorageManagerIF::RETURN_OK) {
    this->setData(const_cast<uint8_t*>(tempData), tempSize, nullptr);
  } else {
    // To circumvent size checks
    this->setData(nullptr, -1, nullptr);
    this->storeAddress.raw = StorageManagerIF::INVALID_ADDRESS;
  }
}

store_address_t CFDPPacketStored::getStoreAddress() { return this->storeAddress; }

CFDPPacketStored::~CFDPPacketStored() = default;

ReturnValue_t CFDPPacketStored::getData(const uint8_t** dataPtr, size_t* dataSize) {
  return HasReturnvaluesIF::RETURN_OK;
}

// ReturnValue_t CFDPPacketStored::setData(const uint8_t *data) {
//     return HasReturnvaluesIF::RETURN_OK;
// }

bool CFDPPacketStored::checkAndSetStore() {
  if (STORE == nullptr) {
    STORE = ObjectManager::instance()->get<StorageManagerIF>(objects::TC_STORE);
    if (STORE == nullptr) {
#if FSFW_CPP_OSTREAM_ENABLED == 1
      sif::error << "CFDPPacketStored::CFDPPacketStored: TC Store not found!" << std::endl;
#endif
      return false;
    }
  }
  return true;
}

bool CFDPPacketStored::isSizeCorrect() {
  const uint8_t* temp_data = nullptr;
  size_t temp_size;
  ReturnValue_t status = STORE->getData(this->storeAddress, &temp_data, &temp_size);
  if (status == StorageManagerIF::RETURN_OK) {
    if (this->getFullSize() == temp_size) {
      return true;
    }
  }
  return false;
}
