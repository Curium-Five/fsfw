#include "KeepAlivePduDeserializer.h"

KeepAlivePduDeserializer::KeepAlivePduDeserializer(const uint8_t* pduBuf, size_t maxSize,
                                                   cfdp::FileSize& progress)
    : FileDirectiveReader(pduBuf, maxSize), progress(progress) {}

ReturnValue_t KeepAlivePduDeserializer::parseData() {
  ReturnValue_t result = FileDirectiveReader::parseData();
  if (result != HasReturnvaluesIF::RETURN_OK) {
    return result;
  }
  size_t currentIdx = FileDirectiveReader::getHeaderSize();
  size_t remLen = FileDirectiveReader::getWholePduSize() - currentIdx;
  const uint8_t* buffer = rawPtr + currentIdx;
  return progress.deSerialize(&buffer, &remLen, getEndianness());
}

cfdp::FileSize& KeepAlivePduDeserializer::getProgress() { return progress; }
