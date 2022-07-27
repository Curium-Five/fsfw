#ifndef FSFW_TMTCPACKET_TMPACKETCREATOR_H
#define FSFW_TMTCPACKET_TMPACKETCREATOR_H

#include "PusTmIF.h"
#include "fsfw/tmtcpacket/ccsds/SpacePacketCreator.h"
#include "fsfw/tmtcpacket/pus/CustomUserDataIF.h"

struct PusTmSecHeader {
  PusTmSecHeader() = default;
  PusTmSecHeader(uint8_t service, uint8_t subservice, TimeStamperIF* timeStamper)
      : service(service), subservice(subservice), timeStamper(timeStamper) {}

  uint8_t service = 0;
  uint8_t subservice = 0;
  TimeStamperIF* timeStamper = nullptr;
  uint8_t pusVersion = ecss::PusVersion::PUS_C;
  uint8_t scTimeRefStatus = 0;
  uint16_t messageTypeCounter = 0;
  uint16_t destId = 0;
};

struct PusTmParams {
  PusTmParams() = default;
  explicit PusTmParams(PusTmSecHeader secHeader) : secHeader(secHeader){};
  PusTmParams(PusTmSecHeader secHeader, ecss::DataWrapper dataWrapper)
      : secHeader(secHeader), dataWrapper(dataWrapper) {}

  PusTmParams(uint8_t service, uint8_t subservice, TimeStamperIF* timeStamper)
      : secHeader(service, subservice, timeStamper) {}

  PusTmParams(uint8_t service, uint8_t subservice, TimeStamperIF* timeStamper,
              ecss::DataWrapper dataWrapper_)
      : PusTmParams(service, subservice, timeStamper) {
    dataWrapper = dataWrapper_;
  }
  PusTmSecHeader secHeader;
  ecss::DataWrapper dataWrapper{};
};

class TimeStamperIF;

class PusTmCreator : public SerializeIF, public PusTmIF, public CustomUserDataIF {
 public:
  PusTmCreator();
  PusTmCreator(SpacePacketParams initSpParams, PusTmParams initPusParams);
  ~PusTmCreator() override = default;

  void setTimeStamper(TimeStamperIF& timeStamper);
  /**
   * This function disables the CRC16 calculation on serialization. This is useful to avoid
   * duplicate calculation if some lower level component needs to update fields like the sequence
   * count, which would require a checksum update.
   */
  void disableCrcCalculation();
  void enableCrcCalculation();
  [[nodiscard]] bool crcCalculationEnabled() const;

  SpacePacketParams& getSpParams();
  void setApid(uint16_t apid);
  void setDestId(uint16_t destId);
  void setService(uint8_t service);
  void setSubservice(uint8_t subservice);
  void setMessageTypeCounter(uint16_t messageTypeCounter);
  PusTmParams& getParams();
  void updateSpLengthField();
  [[nodiscard]] uint16_t getPacketIdRaw() const override;
  [[nodiscard]] uint16_t getPacketSeqCtrlRaw() const override;
  [[nodiscard]] uint16_t getPacketDataLen() const override;
  [[nodiscard]] uint8_t getPusVersion() const override;
  [[nodiscard]] uint8_t getService() const override;
  [[nodiscard]] uint8_t getSubService() const override;
  uint8_t getScTimeRefStatus() override;
  uint16_t getMessageTypeCounter() override;
  uint16_t getDestId() override;
  // Override these to use default big endianness
  ReturnValue_t serialize(uint8_t** buffer, size_t* size, size_t maxSize) const override;
  ReturnValue_t serialize(uint8_t* buffer, size_t maxSize) const override;
  ReturnValue_t serialize(uint8_t** buffer, size_t* size, size_t maxSize,
                          Endianness streamEndianness) const override;
  [[nodiscard]] size_t getSerializedSize() const override;
  ReturnValue_t deSerialize(const uint8_t** buffer, size_t* size,
                            Endianness streamEndianness) override;
  [[nodiscard]] TimeStamperIF* getTimestamper() const;
  ReturnValue_t setRawUserData(const uint8_t* data, size_t len) override;
  ReturnValue_t setSerializableUserData(SerializeIF& serializable) override;

 private:
  void setup();
  PusTmParams pusParams{};
  bool calculateCrcOnSerialization = true;
  SpacePacketCreator spCreator;
};
#endif  // FSFW_TMTCPACKET_TMPACKETCREATOR_H
