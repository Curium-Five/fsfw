#include "fsfw/tmtcpacket/pus/tm/TmPacketPusA.h"
#include "fsfw/tmtcpacket/pus/tm/TmPacketBase.h"

#include "fsfw/globalfunctions/CRC.h"
#include "fsfw/globalfunctions/arrayprinter.h"
#include "fsfw/objectmanager/ObjectManagerIF.h"
#include "fsfw/serviceinterface/ServiceInterface.h"
#include "fsfw/timemanager/CCSDSTime.h"

#include <cstring>


TmPacketPusA::TmPacketPusA(uint8_t* setData) : TmPacketBase(setData) {
    tmData = reinterpret_cast<TmPacketPointerPusA*>(setData);
}

TmPacketPusA::~TmPacketPusA() {
    //Nothing to do.
}

uint8_t TmPacketPusA::getService() {
    return tmData->data_field.service_type;
}

uint8_t TmPacketPusA::getSubService() {
    return tmData->data_field.service_subtype;
}

uint8_t* TmPacketPusA::getSourceData() {
    return &tmData->data;
}

uint16_t TmPacketPusA::getSourceDataSize() {
    return getPacketDataLength() - sizeof(tmData->data_field)
            - CRC_SIZE + 1;
}

void TmPacketPusA::setData(const uint8_t* p_Data) {
    SpacePacketBase::setData(p_Data);
    tmData = (TmPacketPointerPusA*) p_Data;
}


size_t TmPacketPusA::getPacketMinimumSize() const {
    return TM_PACKET_MIN_SIZE;
}

uint16_t TmPacketPusA::getDataFieldSize() {
    return sizeof(PUSTmDataFieldHeaderPusA);
}

uint8_t* TmPacketPusA::getPacketTimeRaw() const {
    return tmData->data_field.time;

}

void TmPacketPusA::initializeTmPacket(uint16_t apid, uint8_t service,
        uint8_t subservice, uint8_t packetSubcounter) {
    //Set primary header:
    initSpacePacketHeader(false, true, apid);
    //Set data Field Header:
    //First, set to zero.
    memset(&tmData->data_field, 0, sizeof(tmData->data_field));

    tmData->data_field.version_type_ack = TM_PUS_VERSION_NUMBER << 4;
    tmData->data_field.service_type = service;
    tmData->data_field.service_subtype = subservice;
    tmData->data_field.subcounter = packetSubcounter;
    //Timestamp packet
    if (TmPacketBase::checkAndSetStamper()) {
        timeStamper->addTimeStamp(tmData->data_field.time,
                sizeof(tmData->data_field.time));
    }
}

void TmPacketPusA::setSourceDataSize(uint16_t size) {
    setPacketDataLength(size + sizeof(PUSTmDataFieldHeaderPusA) + CRC_SIZE - 1);
}

size_t TmPacketPusA::getTimestampSize() const {
    return sizeof(tmData->data_field.time);
}
