#include "fsfw/FSFW.h"
#include "fsfw/tcdistribution/CcsdsDistributor.h"

#include "definitions.h"
#include "fsfw/objectmanager/ObjectManager.h"
#include "fsfw/serviceinterface/ServiceInterface.h"
#include "fsfw/tmtcpacket/ccsds/SpacePacketReader.h"

#define CCSDS_DISTRIBUTOR_DEBUGGING 0

CcsdsDistributor::CcsdsDistributor(uint16_t setDefaultApid, object_id_t setObjectId,
                                   CcsdsPacketCheckIF* packetChecker)
    : TcDistributorBase(setObjectId),
      defaultApid(setDefaultApid),
      packetChecker(packetChecker) {}

CcsdsDistributor::~CcsdsDistributor() = default;

ReturnValue_t CcsdsDistributor::selectDestination(MessageQueueId_t& destId) {
#if CCSDS_DISTRIBUTOR_DEBUGGING == 1
#if FSFW_CPP_OSTREAM_ENABLED == 1
  sif::debug << "CCSDSDistributor::selectDestination received: "
             << this->currentMessage.getStorageId().poolIndex << ", "
             << this->currentMessage.getStorageId().packetIndex << std::endl;
#else
  sif::printDebug("CCSDSDistributor::selectDestination received: %d, %d\n",
                  currentMessage.getStorageId().poolIndex,
                  currentMessage.getStorageId().packetIndex);
#endif
#endif
  const uint8_t* packet = nullptr;
  size_t size = 0;
  ReturnValue_t result = tcStore->getData(currentMessage.getStorageId(), &packet, &size);
  if (result != HasReturnvaluesIF::RETURN_OK) {
#if FSFW_VERBOSE_LEVEL >= 1
#if FSFW_CPP_OSTREAM_ENABLED == 1
    sif::error << "CCSDSDistributor::selectDestination: Getting data from"
                  " store failed!"
               << std::endl;
#else
    sif::printError(
        "CCSDSDistributor::selectDestination: Getting data from"
        " store failed!\n");
#endif
#endif
    return result;
  }
  SpacePacketReader currentPacket(packet, size);
  result = packetChecker->checkPacket(currentPacket, size);
  if (result != HasReturnvaluesIF::RETURN_OK) {
    handlePacketCheckFailure(result);
    return result;
  }
#if FSFW_CPP_OSTREAM_ENABLED == 1 && CCSDS_DISTRIBUTOR_DEBUGGING == 1
  sif::info << "CCSDSDistributor::selectDestination has packet with APID 0x" << std::hex
            << currentPacket.getApid() << std::dec << std::endl;
#endif
  auto iter = receiverMap.find(currentPacket.getApid());
  if (iter != receiverMap.end()) {
    destId = iter->second.destId;
    if (iter->second.removeHeader) {
      handleCcsdsHeaderRemoval();
    }
  } else {
    // The APID was not found. Forward packet to main SW-APID anyway to
    //  create acceptance failure report.
    iter = receiverMap.find(defaultApid);
    if (iter != receiverMap.end()) {
      destId = iter->second.destId;
    } else {
      return DESTINATION_NOT_FOUND;
    }
  }
  return HasReturnvaluesIF::RETURN_OK;
}

void CcsdsDistributor::handlePacketCheckFailure(ReturnValue_t result) {
#if FSFW_VERBOSE_LEVEL >= 1
  const char* reason = "Unknown reason";
  if (result == tcdistrib::INVALID_CCSDS_VERSION) {
    reason = "Invalid CCSDS version";
  } else if (result == tcdistrib::INCOMPLETE_PACKET) {
    reason = "Size missmatch between CCSDS  data length and packet length";
  } else if (result == tcdistrib::INVALID_APID) {
    reason = "No valid handler APID found";
  } else if (result == tcdistrib::INVALID_PACKET_TYPE) {
    reason = "Invalid Packet Type TM detected";
  }
#if FSFW_CPP_OSTREAM_ENABLED == 1
  sif::warning << "CCSDS packet check failed: " << reason << std::endl;
#else
  sif::printWarning("CCSDS packet check failed: %s\n", reason);
#endif
#endif
}

MessageQueueId_t CcsdsDistributor::getRequestQueue() const { return tcQueue->getId(); }

ReturnValue_t CcsdsDistributor::registerApplication(DestInfo info) {
  ReturnValue_t returnValue = RETURN_OK;
  auto insertPair = receiverMap.emplace(info.apid, info);
  if (not insertPair.second) {
    returnValue = RETURN_FAILED;
  }
  return returnValue;
}

uint32_t CcsdsDistributor::getIdentifier() const { return 0; }

ReturnValue_t CcsdsDistributor::initialize() {
  if (packetChecker == nullptr) {
    packetChecker = new CcsdsPacketChecker(ccsds::PacketType::TC);
  }
  ReturnValue_t status = this->TcDistributorBase::initialize();
  this->tcStore = ObjectManager::instance()->get<StorageManagerIF>(objects::TC_STORE);
  if (this->tcStore == nullptr) {
#if FSFW_VERBOSE_LEVEL >= 1
#if FSFW_CPP_OSTREAM_ENABLED == 1
    sif::error << "CCSDSDistributor::initialize: Could not initialize"
                  " TC store!"
               << std::endl;
#else
    sif::printError(
        "CCSDSDistributor::initialize: Could not initialize"
        " TC store!\n");
#endif
#endif
    status = RETURN_FAILED;
  }
  return status;
}

ReturnValue_t CcsdsDistributor::callbackAfterSending(ReturnValue_t queueStatus) {
  if (queueStatus != RETURN_OK) {
    tcStore->deleteData(currentMessage.getStorageId());
  }
  return RETURN_OK;
}

void CcsdsDistributor::print() {
#if FSFW_CPP_OSTREAM_ENABLED == 1
  sif::debug << "Distributor content is: " << std::endl << "ID\t| Message Queue ID" << std::endl;
  sif::debug << std::setfill('0') << std::setw(8) << std::hex;
  for (const auto& iter : receiverMap) {
    sif::debug << iter.first << "\t| 0x" << iter.second.destId
               << ", Header Removed: " << std::boolalpha << iter.second.removeHeader << std::endl;
  }
  sif::debug << std::setfill(' ') << std::dec;
#endif
}

const char* CcsdsDistributor::getName() const { return "CCSDS Distributor"; }

ReturnValue_t CcsdsDistributor::handleCcsdsHeaderRemoval() {
  currentMessage;
  auto accessorPair = tcStore->getData(currentMessage.getStorageId());
  if(accessorPair.first != HasReturnvaluesIF::RETURN_OK) {
#if FSFW_CPP_OSTREAM_ENABLED == 1
    sif::error << __func__ << ": Getting TC data failed" << std::endl;
#else
    sif::printError("%s: Getting TC data failed\n", __func__);
#endif
    return accessorPair.first;
  }
}
