#ifndef FSFW_TESTS_PUSSERVICEBASEMOCK_H
#define FSFW_TESTS_PUSSERVICEBASEMOCK_H

#include <queue>

#include "fsfw/tmtcservices/PusServiceBase.h"

class PsbMock : public PusServiceBase {
 public:
  PsbMock(uint8_t service, uint16_t apid, VerificationReporterIF& verifyReporter);
  unsigned int handleRequestCallCnt = 0;
  std::queue<uint8_t> subserviceQueue;
  unsigned int performServiceCallCnt = 0;

  std::pair<bool, ReturnValue_t> handleReqFailPair;
  std::pair<bool, ReturnValue_t> performServiceFailPair;
  ReturnValue_t handleRequest(uint8_t subservice) override;
  ReturnValue_t performService() override;

  void makeNextHandleReqCallFail(ReturnValue_t retval);
  void reset();
};

#endif  // FSFW_TESTS_PUSSERVICEBASEMOCK_H
