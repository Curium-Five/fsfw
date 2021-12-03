#ifndef FSFW_TMTCSERVICES_VERIFICATIONREPORTER_H_
#define FSFW_TMTCSERVICES_VERIFICATIONREPORTER_H_

#include "PusVerificationReport.h"
#include "fsfw/objectmanager/ObjectManagerIF.h"

namespace Factory{
void setStaticFrameworkObjectIds();
}

/**
 * @brief 	This helper object is used to forward verification messages
 * 			which are generated by the Flight Software Framework.
 * @details
 * The messages can be relayed to an arbitrary object, for example a dedicated
 * Verification Reporter. The destination is set by setting the static framework
 * Id VerificationReporter::messageReceiver. The default verification reporter
 * will be the PUS service 1, which sends verification messages according
 * to the PUS standard.
 *
 */
class VerificationReporter {
	friend void (Factory::setStaticFrameworkObjectIds)();
public:
	VerificationReporter();
	virtual ~VerificationReporter();

	void sendSuccessReport( uint8_t set_report_id, TcPacketPusBase* current_packet,
			uint8_t set_step = 0 );
	void sendSuccessReport(uint8_t set_report_id, uint8_t ackFlags,
			uint16_t tcPacketId, uint16_t tcSequenceControl,
			uint8_t set_step = 0);

	void sendFailureReport( uint8_t report_id, TcPacketPusBase* current_packet,
			ReturnValue_t error_code = 0,
			uint8_t step = 0, uint32_t parameter1 = 0,
			uint32_t parameter2 = 0 );
	void sendFailureReport(uint8_t report_id,
			uint8_t ackFlags, uint16_t tcPacketId, uint16_t tcSequenceControl,
			ReturnValue_t error_code = 0, uint8_t step = 0,
			uint32_t parameter1 = 0, uint32_t parameter2 = 0);

	void initialize();

private:
	static object_id_t messageReceiver;
	MessageQueueId_t acknowledgeQueue;
};

#endif /* FSFW_TMTCSERVICES_VERIFICATIONREPORTER_H_ */
