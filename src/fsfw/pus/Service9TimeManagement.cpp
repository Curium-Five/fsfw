#include "fsfw/pus/Service9TimeManagement.h"

#include <cmath>

#include "fsfw/events/EventManagerIF.h"
#include "fsfw/pus/servicepackets/Service9Packets.h"
#include "fsfw/serviceinterface/ServiceInterface.h"
#include "fsfw/timemanager/CCSDSTime.h"

Service9TimeManagement::Service9TimeManagement(PsbParams params) : PusServiceBase(params) {
  params.name = "PUS 9 Time MGMT";
}

Service9TimeManagement::~Service9TimeManagement() = default;

ReturnValue_t Service9TimeManagement::performService() { return returnvalue::OK; }

ReturnValue_t Service9TimeManagement::handleRequest(uint8_t subservice) {
  switch (subservice) {
    case Subservice::SET_TIME: {
      return setTime();
    }
    case Subservice::DUMP_TIME: {
      timeval newTime;
      Clock::getClock_timeval(&newTime);
      uint32_t subsecondMs =
          static_cast<uint32_t>(std::floor(static_cast<double>(newTime.tv_usec) / 1000.0));
      triggerEvent(CLOCK_DUMP, newTime.tv_sec, subsecondMs);
      return returnvalue::OK;
    }
    default:
      return AcceptsTelecommandsIF::INVALID_SUBSERVICE;
  }
}

ReturnValue_t Service9TimeManagement::setTime() {
  Clock::TimeOfDay_t timeToSet;
  TimePacket timePacket(currentPacket.getUserData(), currentPacket.getUserDataLen());
  ReturnValue_t result =
      CCSDSTime::convertFromCcsds(&timeToSet, timePacket.getTime(), timePacket.getTimeSize());
  if (result != returnvalue::OK) {
    triggerEvent(CLOCK_SET_FAILURE, result, 0);
    return result;
  }

  timeval time;
  Clock::getClock_timeval(&time);
  result = Clock::setClock(&timeToSet);

  if (result == returnvalue::OK) {
    timeval newTime;
    Clock::getClock_timeval(&newTime);
    triggerEvent(CLOCK_SET, time.tv_sec, newTime.tv_sec);
    return returnvalue::OK;
  } else {
    triggerEvent(CLOCK_SET_FAILURE, result, 0);
    return returnvalue::FAILED;
  }
}
