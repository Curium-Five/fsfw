#include "SpacePacketCreator.h"

#include <utility>

#include "fsfw/serialize/SerializeAdapter.h"

SpacePacketCreator::SpacePacketCreator(SpacePacketParams params_) : params(std::move(params_)) {
  checkFieldValidity();
}

SpacePacketCreator::SpacePacketCreator(ccsds::PacketType packetType, bool secHeaderFlag,
                                       uint16_t apid, ccsds::SequenceFlags seqFlags,
                                       uint16_t seqCount, uint16_t dataLen, uint8_t version)
    : params(SpacePacketParams(PacketId(packetType, secHeaderFlag, apid),
                               PacketSeqCtrl(seqFlags, seqCount), dataLen)) {
  params.version = version;
}

uint16_t SpacePacketCreator::getPacketIdRaw() const { return params.packetId.raw(); }
uint16_t SpacePacketCreator::getPacketSeqCtrlRaw() const { return params.packetSeqCtrl.raw(); }
uint16_t SpacePacketCreator::getPacketDataLen() const { return params.dataLen; }

ReturnValue_t SpacePacketCreator::serialize(uint8_t **buffer, size_t *size, size_t maxSize,
                                            SerializeIF::Endianness streamEndianness) const {
  if (not isValid()) {
    return HasReturnvaluesIF::RETURN_FAILED;
  }
  uint16_t packetIdAndVersion =
      (static_cast<uint16_t>(params.version) << 13) | params.packetId.raw();
  ReturnValue_t result =
      SerializeAdapter::serialize(&packetIdAndVersion, buffer, size, maxSize, streamEndianness);
  if (result != HasReturnvaluesIF::RETURN_OK) {
    return result;
  }
  uint16_t pscRaw = params.packetSeqCtrl.raw();
  result = SerializeAdapter::serialize(&pscRaw, buffer, size, maxSize, streamEndianness);
  if (result != HasReturnvaluesIF::RETURN_OK) {
    return result;
  }
  return SerializeAdapter::serialize(&params.dataLen, buffer, size, maxSize, streamEndianness);
}

size_t SpacePacketCreator::getSerializedSize() const { return 6; }

ReturnValue_t SpacePacketCreator::deSerialize(const uint8_t **buffer, size_t *size,
                                              SerializeIF::Endianness streamEndianness) {
  return HasReturnvaluesIF::RETURN_FAILED;
}
bool SpacePacketCreator::isValid() const { return valid; }

ReturnValue_t SpacePacketCreator::serialize(uint8_t **buffer, size_t *size, size_t maxSize) const {
  return serialize(buffer, size, maxSize, SerializeIF::Endianness::NETWORK);
}
void SpacePacketCreator::setApid(uint16_t apid) {
  if (apid < ccsds::LIMIT_APID) {
    params.packetId.apid = apid;
  }
  checkFieldValidity();
}

void SpacePacketCreator::setSeqCount(uint16_t seqCount) {
  if (seqCount < ccsds::LIMIT_SEQUENCE_COUNT) {
    params.packetSeqCtrl.seqCount = seqCount;
  }
  checkFieldValidity();
}
void SpacePacketCreator::setSeqFlags(ccsds::SequenceFlags flags) {
  params.packetSeqCtrl.seqFlags = flags;
}
void SpacePacketCreator::setDataLen(uint16_t dataLen_) { params.dataLen = dataLen_; }
void SpacePacketCreator::checkFieldValidity() {
  valid = true;
  if (params.packetId.apid > ccsds::LIMIT_APID or
      params.packetSeqCtrl.seqCount > ccsds::LIMIT_SEQUENCE_COUNT) {
    valid = false;
  }
}

void SpacePacketCreator::setParams(SpacePacketParams params_) { params = std::move(params_); }

SpacePacketParams &SpacePacketCreator::getParams() { return params; }
