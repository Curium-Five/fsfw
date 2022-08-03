#include <array>
#include <catch2/catch_test_macros.hpp>

#include "fsfw/cfdp/pdu/FileDirectiveDeserializer.h"
#include "fsfw/cfdp/pdu/FileDirectiveSerializer.h"

TEST_CASE("CFDP File Directive", "[cfdp]") {
  using namespace cfdp;
  std::array<uint8_t, 32> serBuf{};
  ReturnValue_t result;
  cfdp::TransactionSeqNum seqNum = TransactionSeqNum(cfdp::WidthInBytes::ONE_BYTE, 2);
  cfdp::EntityId sourceId = EntityId(cfdp::WidthInBytes::ONE_BYTE, 0);
  cfdp::EntityId destId = EntityId(cfdp::WidthInBytes::ONE_BYTE, 1);
  PduConfig pduConf =
      PduConfig(sourceId, destId, cfdp::TransmissionModes::ACKNOWLEDGED, seqNum, false);
  uint8_t* serTarget = serBuf.data();
  const uint8_t* deserTarget = serTarget;
  size_t serSize = 0;

  SECTION("File Directive") {
    auto fdSer = FileDirectiveSerializer(pduConf, FileDirectives::ACK, 4);
    REQUIRE(fdSer.getSerializedSize() == 8);
    serTarget = serBuf.data();
    serSize = 0;
    result = fdSer.serialize(&serTarget, &serSize, serBuf.size(), SerializeIF::Endianness::NETWORK);
    REQUIRE(result == HasReturnvaluesIF::RETURN_OK);
    // Only version bits are set
    REQUIRE(serBuf[0] == 0b00100000);
    // PDU data field length is 5 (4 + Directive code octet)
    REQUIRE(serBuf[1] == 0);
    REQUIRE(serBuf[2] == 5);
    // Entity and Transaction Sequence number are 1 byte large
    REQUIRE(serBuf[3] == 0b00010001);
    // Source ID
    REQUIRE(serBuf[4] == 0);
    // Transaction Seq Number
    REQUIRE(serBuf[5] == 2);
    // Dest ID
    REQUIRE(serBuf[6] == 1);
    REQUIRE(serBuf[7] == FileDirectives::ACK);

    serTarget = serBuf.data();
    size_t deserSize = 20;
    serSize = 0;
    REQUIRE(fdSer.deSerialize(&deserTarget, &deserSize, SerializeIF::Endianness::NETWORK) ==
            HasReturnvaluesIF::RETURN_FAILED);
    REQUIRE(fdSer.serialize(nullptr, nullptr, 85, SerializeIF::Endianness::NETWORK) ==
            HasReturnvaluesIF::RETURN_FAILED);
    for (uint8_t idx = 0; idx < 8; idx++) {
      serTarget = serBuf.data();
      serSize = 0;
      REQUIRE(fdSer.serialize(&serTarget, &serSize, idx, SerializeIF::Endianness::NETWORK) ==
              SerializeIF::BUFFER_TOO_SHORT);
    }

    deserTarget = serBuf.data();
    deserSize = 0;
    auto fdDeser = FileDirectiveDeserializer(deserTarget, serBuf.size());
    REQUIRE(fdDeser.getEndianness() == SerializeIF::Endianness::NETWORK);
    fdDeser.setEndianness(SerializeIF::Endianness::MACHINE);
    REQUIRE(fdDeser.getEndianness() == SerializeIF::Endianness::MACHINE);
    fdDeser.setEndianness(SerializeIF::Endianness::NETWORK);
    REQUIRE(fdDeser.parseData() == HasReturnvaluesIF::RETURN_OK);
    REQUIRE(fdDeser.getFileDirective() == FileDirectives::ACK);
    REQUIRE(fdDeser.getPduDataFieldLen() == 5);
    REQUIRE(fdDeser.getHeaderSize() == 8);
    REQUIRE(fdDeser.getPduType() == cfdp::PduType::FILE_DIRECTIVE);

    serBuf[7] = 0xff;
    // Invalid file directive
    REQUIRE(fdDeser.parseData() == cfdp::INVALID_DIRECTIVE_FIELDS);
  }
}