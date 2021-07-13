#include "fsfw/timemanager/Countdown.h"

Countdown::Countdown(uint32_t initialTimeout): timeout(initialTimeout) {
}

Countdown::~Countdown() {
}

ReturnValue_t Countdown::setTimeout(uint32_t miliseconds) {
	ReturnValue_t return_value = Clock::getUptime( &startTime );
	timeout = miliseconds;
	return return_value;
}

bool Countdown::hasTimedOut() const {
	uint32_t current_time;
	Clock::getUptime( &current_time );
	if ( uint32_t(current_time - startTime) >= timeout) {
		return true;
	} else {
		return false;
	}
}

bool Countdown::isBusy() const {
	return !hasTimedOut();
}

ReturnValue_t Countdown::resetTimer() {
	return setTimeout(timeout);
}

void Countdown::timeOut() {
	uint32_t current_time;
		Clock::getUptime( &current_time );
	startTime= current_time - timeout;
}
