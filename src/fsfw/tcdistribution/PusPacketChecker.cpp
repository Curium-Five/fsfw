#include "fsfw/tcdistribution/PusPacketChecker.h"

#include "fsfw/globalfunctions/CRC.h"
#include "fsfw/serviceinterface/ServiceInterface.h"
#include "fsfw/tcdistribution/definitions.h"
#include "fsfw/tmtcpacket/pus/tc/PusTcReader.h"

PusPacketChecker::PusPacketChecker(uint16_t apid, ccsds::PacketType packetType_,
                                   ecss::PusVersion pusVersion_)
    : pusVersion(pusVersion_), apid(apid) {}

ReturnValue_t PusPacketChecker::checkPacket(const PusTcReader& pusPacket, size_t packetLen) {
  // Other primary header fields are checked by base class
  if (not pusPacket.hasSecHeader()) {
    return tcdistrib::INVALID_SEC_HEADER_FIELD;
  }
  uint16_t calculated_crc = CRC::crc16ccitt(pusPacket.getFullData(), pusPacket.getFullPacketLen());
  if (calculated_crc != 0) {
    return tcdistrib::INCORRECT_CHECKSUM;
  }
  if (pusPacket.getApid() != apid) {
    return tcdistrib::INVALID_APID;
  }
  if (pusPacket.getPusVersion() != pusVersion) {
    return tcdistrib::INVALID_PUS_VERSION;
  }
  return HasReturnvaluesIF::RETURN_OK;
}

uint16_t PusPacketChecker::getApid() const { return apid; }
