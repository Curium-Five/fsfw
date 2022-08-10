#include <catch2/catch_test_macros.hpp>

#include "fsfw/cfdp.h"
#include "mocks/cfdp/FaultHandlerMock.h"
#include "mocks/cfdp/UserMock.h"
TEST_CASE("CFDP Dest Handler", "[cfdp]") {
  using namespace cfdp;
  EntityId localId = EntityId(UnsignedByteField<uint16_t>(2));
  auto fhMock = FaultHandlerMock();
  auto localEntityCfg = LocalEntityCfg(localId, IndicationCfg(), fhMock);
  // auto userMock = UserMock();

  SECTION("State") {}
}