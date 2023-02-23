#ifndef FSFW_PUS_SERVICE9TIMEMANAGEMENT_H_
#define FSFW_PUS_SERVICE9TIMEMANAGEMENT_H_

#include "fsfw/tmtcservices/PusServiceBase.h"

class Service9TimeManagement : public PusServiceBase {
 public:
  static constexpr uint8_t SUBSYSTEM_ID = SUBSYSTEM_ID::PUS_SERVICE_9;

  //!< Clock has been set. P1: old timeval seconds. P2: new timeval seconds.
  static constexpr Event CLOCK_SET = MAKE_EVENT(0, severity::INFO);
  //!< Clock dump event. P1: timeval seconds P2: timeval milliseconds.
  static constexpr Event CLOCK_DUMP = MAKE_EVENT(1, severity::INFO);
  //!< Clock could not be set. P1: Returncode.
  static constexpr Event CLOCK_SET_FAILURE = MAKE_EVENT(2, severity::LOW);

  static constexpr uint8_t CLASS_ID = CLASS_ID::PUS_SERVICE_9;

  /**
   * 	@brief This service provides the capability to set the on-board time.
   */
  explicit Service9TimeManagement(PsbParams params);

  ~Service9TimeManagement() override;

  ReturnValue_t performService() override;

  /**
   * 	@brief Sets the onboard-time by retrieving the time to set from TC[9,128].
   */
  ReturnValue_t handleRequest(uint8_t subservice) override;

  virtual ReturnValue_t setTime();

 private:
  enum Subservice {
    SET_TIME = 128,  //!< [EXPORT] : [COMMAND] Time command in ASCII, CUC or CDS format
    DUMP_TIME = 129,
  };
};

#endif /* FSFW_PUS_SERVICE9TIMEMANAGEMENT_H_ */
