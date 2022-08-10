#include "UserMock.h"

cfdp::UserMock::UserMock(HasFileSystemIF& vfs) : UserBase(vfs) {}

void cfdp::UserMock::transactionIndication(cfdp::TransactionId id) {}
void cfdp::UserMock::eofSentIndication(cfdp::TransactionId id) {}
void cfdp::UserMock::abandonedIndication(cfdp::TransactionId id, cfdp::ConditionCode code,
                                         uint64_t progress) {}
void cfdp::UserMock::eofRecvIndication(cfdp::TransactionId id) {}
void cfdp::UserMock::transactionFinishedIndication(TransactionFinishedParams finishedParams) {}
void cfdp::UserMock::metadataRecvdIndication(MetadataRecvParams params) {}
void cfdp::UserMock::fileSegmentRecvdIndication(FileSegmentRecvdParams params) {}
void cfdp::UserMock::reportIndication() {}
void cfdp::UserMock::suspendedIndication() {}
void cfdp::UserMock::resumedIndication() {}
