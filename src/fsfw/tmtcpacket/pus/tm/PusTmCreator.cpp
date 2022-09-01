#include "PusTmCreator.h"

#include <utility>

#include "fsfw/globalfunctions/CRC.h"
#include "fsfw/timemanager/TimeStamperIF.h"

PusTmCreator::PusTmCreator(SpacePacketParams initSpParams, PusTmParams initPusParams)
    : pusParams(initPusParams), spCreator(std::move(initSpParams)) {
  setup();
}

PusTmCreator::PusTmCreator() { setup(); }

void PusTmCreator::disableCrcCalculation() { calculateCrcOnSerialization = false; }

void PusTmCreator::enableCrcCalculation() { calculateCrcOnSerialization = true; }

uint16_t PusTmCreator::getPacketIdRaw() const { return spCreator.getPacketIdRaw(); }

uint16_t PusTmCreator::getPacketSeqCtrlRaw() const { return spCreator.getPacketSeqCtrlRaw(); }

uint16_t PusTmCreator::getPacketDataLen() const { return spCreator.getPacketDataLen(); }

uint8_t PusTmCreator::getPusVersion() const { return pusParams.secHeader.pusVersion; }

uint8_t PusTmCreator::getService() const { return pusParams.secHeader.service; }

uint8_t PusTmCreator::getSubService() const { return pusParams.secHeader.subservice; }

PusTmParams& PusTmCreator::getParams() { return pusParams; }

void PusTmCreator::setTimeStamper(TimeStamperIF& timeStamper_) {
  pusParams.secHeader.timeStamper = &timeStamper_;
  updateSpLengthField();
}

uint8_t PusTmCreator::getScTimeRefStatus() { return pusParams.secHeader.scTimeRefStatus; }

uint16_t PusTmCreator::getMessageTypeCounter() { return pusParams.secHeader.messageTypeCounter; }

uint16_t PusTmCreator::getDestId() { return pusParams.secHeader.destId; }

ReturnValue_t PusTmCreator::serialize(uint8_t** buffer, size_t* size, size_t maxSize,
                                      SerializeIF::Endianness streamEndianness) const {
  const uint8_t* start = *buffer;
  if (*size + getSerializedSize() > maxSize) {
    return SerializeIF::BUFFER_TOO_SHORT;
  }
  ReturnValue_t result = spCreator.serialize(buffer, size, maxSize, streamEndianness);
  if (result != returnvalue::OK) {
    return result;
  }
  size_t userDataLen = pusParams.dataWrapper.getLength();
  **buffer =
      ((pusParams.secHeader.pusVersion << 4) & 0xF0) | (pusParams.secHeader.scTimeRefStatus & 0x0F);
  *buffer += 1;
  **buffer = pusParams.secHeader.service;
  *buffer += 1;
  **buffer = pusParams.secHeader.subservice;
  *buffer += 1;
  *size += 3;
  result = SerializeAdapter::serialize(&pusParams.secHeader.messageTypeCounter, buffer, size,
                                       maxSize, streamEndianness);
  if (result != returnvalue::OK) {
    return result;
  }
  result = SerializeAdapter::serialize(&pusParams.secHeader.destId, buffer, size, maxSize,
                                       streamEndianness);
  if (result != returnvalue::OK) {
    return result;
  }
  if (getTimestamper() != nullptr) {
    result = pusParams.secHeader.timeStamper->serialize(buffer, size, maxSize, streamEndianness);
    if (result != returnvalue::OK) {
      return result;
    }
  }

  if (pusParams.dataWrapper.type == util::DataTypes::RAW and
      pusParams.dataWrapper.dataUnion.raw.data != nullptr) {
    std::memcpy(*buffer, pusParams.dataWrapper.dataUnion.raw.data, userDataLen);
    *buffer += userDataLen;
    *size += userDataLen;
  } else if (pusParams.dataWrapper.type == util::DataTypes::SERIALIZABLE and
             pusParams.dataWrapper.dataUnion.serializable != nullptr) {
    result = pusParams.dataWrapper.dataUnion.serializable->serialize(buffer, size, maxSize,
                                                                     streamEndianness);
    if (result != returnvalue::OK) {
      return result;
    }
  }
  if (calculateCrcOnSerialization) {
    uint16_t crc16 = CRC::crc16ccitt(start, getFullPacketLen() - sizeof(ecss::PusChecksumT));
    return SerializeAdapter::serialize(&crc16, buffer, size, maxSize, streamEndianness);
  }
  // Even if no CRC is calculated, account for the space taken by it
  *size += 2;
  *buffer += 2;
  return returnvalue::OK;
}

size_t PusTmCreator::getSerializedSize() const { return getFullPacketLen(); }
ReturnValue_t PusTmCreator::deSerialize(const uint8_t** buffer, size_t* size,
                                        SerializeIF::Endianness streamEndianness) {
  return returnvalue::FAILED;
}

TimeStamperIF* PusTmCreator::getTimestamper() const { return pusParams.secHeader.timeStamper; }

SpacePacketParams& PusTmCreator::getSpParams() { return spCreator.getParams(); }

void PusTmCreator::updateSpLengthField() {
  size_t headerLen = PusTmIF::MIN_SEC_HEADER_LEN + pusParams.dataWrapper.getLength() +
                     sizeof(ecss::PusChecksumT) - 1;
  if (pusParams.secHeader.timeStamper != nullptr) {
    headerLen += pusParams.secHeader.timeStamper->getSerializedSize();
  }
  spCreator.setDataLen(headerLen);
}

void PusTmCreator::setApid(uint16_t apid) { spCreator.setApid(apid); }

void PusTmCreator::setup() {
  updateSpLengthField();
  spCreator.setPacketType(ccsds::PacketType::TM);
  spCreator.setSecHeaderFlag();
}

void PusTmCreator::setMessageTypeCounter(uint16_t messageTypeCounter) {
  pusParams.secHeader.messageTypeCounter = messageTypeCounter;
};

void PusTmCreator::setDestId(uint16_t destId) { pusParams.secHeader.destId = destId; }

ReturnValue_t PusTmCreator::setRawUserData(const uint8_t* data, size_t len) {
  pusParams.dataWrapper.setRawData({data, len});
  updateSpLengthField();
  return returnvalue::OK;
}
ReturnValue_t PusTmCreator::setSerializableUserData(SerializeIF& serializable) {
  pusParams.dataWrapper.setSerializable(serializable);
  updateSpLengthField();
  return returnvalue::OK;
}

void PusTmCreator::setService(uint8_t service) { pusParams.secHeader.service = service; }

void PusTmCreator::setSubservice(uint8_t subservice) {
  pusParams.secHeader.subservice = subservice;
}
bool PusTmCreator::crcCalculationEnabled() const { return calculateCrcOnSerialization; }
