#include <catch2/catch_test_macros.hpp>

#include "fsfw/cfdp.h"
#include "mocks/CfdpFaultHandlerMock.h"

TEST_CASE("CFDP Dest Handler", "[cfdp]") {
  using namespace cfdp;
  EntityId localId = EntityId(UnsignedByteField<uint16_t>(2));
  auto fhMock = CfdpFaultHandlerMock();
  auto localEntityCfg = LocalEntityCfg(localId, IndicationCfg(), fhMock);
  // auto destHandler = DestHandler();

  SECTION("State") {}
}