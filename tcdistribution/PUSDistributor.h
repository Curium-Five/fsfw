#ifndef FRAMEWORK_TCDISTRIBUTION_PUSDISTRIBUTOR_H_
#define FRAMEWORK_TCDISTRIBUTION_PUSDISTRIBUTOR_H_

#include <framework/returnvalues/HasReturnvaluesIF.h>
#include <framework/tcdistribution/PUSDistributorIF.h>
#include <framework/tcdistribution/TcDistributor.h>
#include <framework/tcdistribution/TcPacketCheck.h>
#include <framework/tmtcservices/AcceptsTelecommandsIF.h>
#include <framework/tmtcservices/VerificationReporter.h>

/**
 * This class accepts PUS Telecommands and forwards them to Application
 * services. In addition, the class performs a formal packet check and
 * sends acceptance success or failure messages.
 * @ingroup tc_distribution
 */
class PUSDistributor: public TcDistributor,
		public PUSDistributorIF,
		public AcceptsTelecommandsIF {
public:
	/**
	 * The ctor passes @c set_apid to the checker class and calls the
	 * TcDistribution ctor with a certain object id.
	 * @param setApid The APID of this receiving Application.
	 * @param setObjectId Object ID of the distributor itself
	 * @param setPacketSource Object ID of the source of TC packets.
	 * Must implement CCSDSDistributorIF.
	 */
	PUSDistributor(uint16_t setApid, object_id_t setObjectId,
			object_id_t setPacketSource);
	/**
	 * The destructor is empty.
	 */
	virtual ~PUSDistributor();
	ReturnValue_t registerService(AcceptsTelecommandsIF* service) override;
	MessageQueueId_t getRequestQueue() override;
	ReturnValue_t initialize() override;
	uint16_t getIdentifier() override;

protected:
	/**
	 * This attribute contains the class, that performs a formal packet check.
	 */
	TcPacketCheck checker;
	/**
	 * With this class, verification messages are sent to the
	 * TC Verification service.
	 */
	VerificationReporter verifyChannel;
	/**
	 * The currently handled packet is stored here.
	 */
	TcPacketStored currentPacket;
	/**
	 * With this variable, the current check status is stored to generate
	 * acceptance messages later.
	 */
	ReturnValue_t tcStatus;

	const object_id_t packetSource;
	/**
	 * This method reads the packet service, checks if such a service is
	 * registered and forwards the packet to the destination.
	 * It also initiates the formal packet check and sending of verification
	 * messages.
	 * @return Iterator to map entry of found service id
	 * or iterator to @c map.end().
	 */
	TcMessageQueueMapIter selectDestination();
	/**
	 * The callback here handles the generation of acceptance
	 * success/failure messages.
	 */
	ReturnValue_t callbackAfterSending(ReturnValue_t queueStatus);
};

#endif /* FRAMEWORK_TCDISTRIBUTION_PUSDISTRIBUTOR_H_ */
