/*
 * EventCorrelation.h
 *
 *  Created on: 15.10.2015
 *      Author: baetz
 */

#ifndef FRAMEWORK_FDIR_EVENTCORRELATION_H_
#define FRAMEWORK_FDIR_EVENTCORRELATION_H_

#include <framework/timemanager/Countdown.h>

class EventCorrelation {
public:
	enum State {
		CORRELATION_STARTED,
		CORRELATED,
		ALREADY_STARTED
	};
	EventCorrelation(uint32_t timeout);
	~EventCorrelation();
	EventCorrelation::State doesEventCorrelate();
	bool isEventPending();
	bool hasPendingEventTimedOut();
	Countdown correlationTimer;
private:
	bool eventPending;
};

#endif /* FRAMEWORK_FDIR_EVENTCORRELATION_H_ */
