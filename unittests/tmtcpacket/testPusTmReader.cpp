#include <catch2/catch_test_macros.hpp>

#include "fsfw/tmtcpacket/pus/tm/PusTmCreator.h"
#include "fsfw/tmtcpacket/pus/tm/PusTmReader.h"
#include "mocks/CdsShortTimestamperMock.h"

TEST_CASE("PUS TM Reader", "[pus-tm-reader]") {
  auto packetId = PacketId(ccsds::PacketType::TC, true, 0xef);
  auto spParams =
      SpacePacketParams(packetId, PacketSeqCtrl(ccsds::SequenceFlags::UNSEGMENTED, 22), 0x00);
  auto timeStamperAndReader = CdsShortTimestamperMock();
  auto pusTmParams = PusTmParams(17, 2, &timeStamperAndReader);
  timeStamperAndReader.valueToStamp = {1, 2, 3, 4, 5, 6, 7};
  PusTmCreator creator(spParams, pusTmParams);
  PusTmReader reader(&timeStamperAndReader);
  std::array<uint8_t, 32> buf{};
  uint8_t* dataPtr = buf.data();
  size_t serLen = 0;

  SECTION("Basic") {
    PusTmReader* readerPtr = &reader;
    bool deleteReader = false;
    dataPtr = buf.data();
    serLen = 0;
    REQUIRE(creator.serialize(&dataPtr, &serLen, buf.size()) == HasReturnvaluesIF::RETURN_OK);
    REQUIRE(not(*readerPtr));
    REQUIRE(readerPtr->isNull());

    SECTION("Setter") {
      REQUIRE(readerPtr->setReadOnlyData(buf.data(), serLen) == HasReturnvaluesIF::RETURN_OK);
    }
    SECTION("Full Construction") {
      readerPtr = new PusTmReader(&timeStamperAndReader, buf.data(), serLen);
      deleteReader = true;
    }
    SECTION("Time Stamper set manually") {
      readerPtr = new PusTmReader(buf.data(), serLen);
      readerPtr->setTimeReader(&timeStamperAndReader);
      deleteReader = true;
    }
    REQUIRE(readerPtr);
    REQUIRE(not readerPtr->isNull());
    REQUIRE(readerPtr->parseDataWithCrcCheck() == HasReturnvaluesIF::RETURN_OK);
    REQUIRE(readerPtr->getService() == 17);
    REQUIRE(readerPtr->getSubService() == 2);
    REQUIRE(readerPtr->getApid() == 0xef);
    REQUIRE(readerPtr->getSequenceFlags() == ccsds::SequenceFlags::UNSEGMENTED);
    REQUIRE(readerPtr->getScTimeRefStatus() == 0);
    REQUIRE(readerPtr->getDestId() == 0);
    REQUIRE(readerPtr->getMessageTypeCounter() == 0);
    REQUIRE(readerPtr->getTimeReader() == &timeStamperAndReader);
    REQUIRE(readerPtr->getPusVersion() == 2);
    REQUIRE(readerPtr->getPacketDataLen() == 15);
    REQUIRE(readerPtr->getFullPacketLen() == 22);
    REQUIRE(readerPtr->getUserDataLen() == 0);
    REQUIRE(readerPtr->getUserData() == nullptr);
    REQUIRE(readerPtr->getFullData() == buf.data());
    if (deleteReader) {
      delete readerPtr;
    }
  }

  SECTION("Invalid CRC") {
    REQUIRE(creator.serialize(&dataPtr, &serLen, buf.size()) == HasReturnvaluesIF::RETURN_OK);
    buf[20] = 0;
    REQUIRE(reader.setReadOnlyData(buf.data(), serLen) == HasReturnvaluesIF::RETURN_OK);
    REQUIRE(reader.parseDataWithCrcCheck() == PusIF::INVALID_CRC_16);
  }

  SECTION("Set Time Reader") {
    reader.setTimeReader(nullptr);
    REQUIRE(reader.getTimeReader() == nullptr);
  }

  SECTION("Invalid CRC ignored") {
    REQUIRE(creator.serialize(&dataPtr, &serLen, buf.size()) == HasReturnvaluesIF::RETURN_OK);
    buf[20] = 0;
    REQUIRE(reader.setReadOnlyData(buf.data(), serLen) == HasReturnvaluesIF::RETURN_OK);
    REQUIRE(reader.parseDataWithoutCrcCheck() == HasReturnvaluesIF::RETURN_OK);
  }

  SECTION("Read with source data") {
    std::array<uint8_t, 3> data{1, 2, 3};
    creator.setRawUserData(data.data(), data.size());
    REQUIRE(creator.serialize(&dataPtr, &serLen, buf.size()) == HasReturnvaluesIF::RETURN_OK);
    REQUIRE(reader.setReadOnlyData(buf.data(), serLen) == HasReturnvaluesIF::RETURN_OK);
    REQUIRE(reader.parseDataWithCrcCheck() == HasReturnvaluesIF::RETURN_OK);
    REQUIRE(reader.getUserDataLen() == 3);
    const uint8_t* roData = reader.getUserData();
    REQUIRE(roData[0] == 1);
    REQUIRE(roData[1] == 2);
    REQUIRE(roData[2] == 3);
  }
}