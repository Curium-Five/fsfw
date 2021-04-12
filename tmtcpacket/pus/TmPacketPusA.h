#ifndef FSFW_TMTCPACKET_PUS_TMPACKETPUSA_H_
#define FSFW_TMTCPACKET_PUS_TMPACKETPUSA_H_

#include "TmPacketBase.h"
#include "../SpacePacketBase.h"
#include "../../timemanager/TimeStamperIF.h"
#include "../../timemanager/Clock.h"
#include "../../objectmanager/SystemObjectIF.h"

namespace Factory{
void setStaticFrameworkObjectIds();
}

/**
 * This struct defines a byte-wise structured PUS TM Data Field Header.
 * Any optional fields in the header must be added or removed here.
 * Currently, no Destination field is present, but an eigth-byte representation
 * for a time tag.
 * @ingroup tmtcpackets
 */
struct PUSTmDataFieldHeader {
    uint8_t version_type_ack;
    uint8_t service_type;
    uint8_t service_subtype;
    uint8_t subcounter;
//  uint8_t destination;
    uint8_t time[TimeStamperIF::MISSION_TIMESTAMP_SIZE];
};

/**
 * This struct defines the data structure of a PUS Telecommand Packet when
 * accessed via a pointer.
 * @ingroup tmtcpackets
 */
struct TmPacketPointer {
    CCSDSPrimaryHeader primary;
    PUSTmDataFieldHeader data_field;
    uint8_t data;
};

/**
 * PUS A packet implementation
 * @ingroup tmtcpackets
 */
class TmPacketPusA: public TmPacketBase {
    friend void (Factory::setStaticFrameworkObjectIds)();
public:
    /**
     * This constant defines the minimum size of a valid PUS Telemetry Packet.
     */
    static const uint32_t  TM_PACKET_MIN_SIZE = (sizeof(CCSDSPrimaryHeader) +
            sizeof(PUSTmDataFieldHeader) + 2);
    //! Maximum size of a TM Packet in this mission.
    //! TODO: Make this dependant on a config variable.
    static const uint32_t MISSION_TM_PACKET_MAX_SIZE = 2048;
    //! First byte of secondary header for PUS-A packets.
    //! TODO: Maybe also support PUS-C via config?
    static const uint8_t VERSION_NUMBER_BYTE_PUS_A = 0b00010000;

    /**
     * This is the default constructor.
     * It sets its internal data pointer to the address passed and also
     * forwards the data pointer to the parent SpacePacketBase class.
     * @param set_address   The position where the packet data lies.
     */
    TmPacketPusA( uint8_t* setData );
    /**
     * This is the empty default destructor.
     */
    virtual ~TmPacketPusA();

    /* TmPacketBase implementations */
    uint8_t getService() override;
    uint8_t getSubService() override;
    uint8_t* getSourceData() override;
    uint16_t getSourceDataSize() override;
    uint16_t getDataFieldSize() override;
    /**
     * Interprets the "time"-field in the secondary header and returns it in
     * timeval format.
     * @return Converted timestamp of packet.
     */
    ReturnValue_t getPacketTime(timeval* timestamp) const override;
    /**
     * Returns a raw pointer to the beginning of the time field.
     * @return Raw pointer to time field.
     */
    uint8_t* getPacketTimeRaw() const override;

    size_t getTimestampSize() const override;
    size_t getPacketMinimumSize() const override;

protected:
    /**
     * A pointer to a structure which defines the data structure of
     * the packet's data.
     *
     * To be hardware-safe, all elements are of byte size.
     */
    TmPacketPointer* tmData;

    /**
     * Initializes the Tm Packet header.
     * Does set the timestamp (to now), but not the error control field.
     * @param apid APID used.
     * @param service   PUS Service
     * @param subservice PUS Subservice
     * @param packetSubcounter Additional subcounter used.
     */
    void initializeTmPacket(uint16_t apid, uint8_t service, uint8_t subservice,
            uint8_t packetSubcounter);

    /**
     * With this method, the packet data pointer can be redirected to another
     * location.
     *
     * This call overwrites the parent's setData method to set both its
     * @c tc_data pointer and the parent's @c data pointer.
     *
     * @param p_data    A pointer to another PUS Telemetry Packet.
     */
    void setData( const uint8_t* pData );

    /**
     * In case data was filled manually (almost never the case).
     * @param size Size of source data (without CRC and data filed header!).
     */
    void setSourceDataSize(uint16_t size);

    /**
     * Checks if a time stamper is available and tries to set it if not.
     * @return Returns false if setting failed.
     */
    bool checkAndSetStamper();
};

#endif /* FSFW_TMTCPACKET_PUS_TMPACKETPUSA_H_ */
