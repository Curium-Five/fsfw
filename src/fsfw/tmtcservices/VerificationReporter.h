#ifndef FSFW_TMTCSERVICES_VERIFICATIONREPORTER_H_
#define FSFW_TMTCSERVICES_VERIFICATIONREPORTER_H_

#include "PusVerificationReport.h"
#include "VerificationReporterIF.h"
#include "fsfw/objectmanager/ObjectManagerIF.h"
#include "fsfw/objectmanager/SystemObject.h"
#include "fsfw/tmtcpacket/pus/tc/PusTcCreator.h"
#include "fsfw/tmtcservices/AcceptsVerifyMessageIF.h"

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
class VerificationReporter : public SystemObject, public VerificationReporterIF {
 public:
  explicit VerificationReporter(AcceptsVerifyMessageIF* receiver,
                                object_id_t objectId = objects::TC_VERIFICATOR);
  ~VerificationReporter() override;

  void setReceiver(AcceptsVerifyMessageIF& receiver);

  // TODO: The API is a little bit bloated. It might be better to group all the parameters
  //       into a dedicated struct
  ReturnValue_t sendSuccessReport(VerifSuccessParams params) override;

  ReturnValue_t sendFailureReport(VerifFailureParams params) override;

 private:
  MessageQueueId_t acknowledgeQueue;
};

#endif /* FSFW_TMTCSERVICES_VERIFICATIONREPORTER_H_ */
