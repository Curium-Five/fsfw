#ifndef FSFW_TESTS_CFDP_USERMOCK_H
#define FSFW_TESTS_CFDP_USERMOCK_H

#include "fsfw/cfdp/handler/UserBase.h"

namespace cfdp {

class UserMock: public UserBase {
  explicit UserMock(HasFileSystemIF& vfs);
 public:
  void transactionIndication(TransactionId id) override;
  void eofSentIndication(TransactionId id) override;
  void abandonedIndication(TransactionId id, ConditionCode code, uint64_t progress) override;
  void eofRecvIndication(TransactionId id) override;
  void transactionFinishedIndication() override;
  void metadataRecvdIndication() override;
  void fileSegmentRecvdIndication() override;
  void reportIndication() override;
  void suspendedIndication() override;
  void resumedIndication() override;
};

}

#endif  // FSFW_TESTS_CFDP_USERMOCK_H
