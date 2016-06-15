/*
 * EventMatchTree.h
 *
 *  Created on: 27.08.2015
 *      Author: baetz
 */

#ifndef FRAMEWORK_EVENTS_EVENTMATCHING_EVENTMATCHTREE_H_
#define FRAMEWORK_EVENTS_EVENTMATCHING_EVENTMATCHTREE_H_

#include <framework/container/PlacementFactory.h>
#include <framework/events/EventMessage.h>
#include <framework/globalfunctions/matching/MatchTree.h>
#include <framework/returnvalues/HasReturnvaluesIF.h>
class StorageManagerIF;

class EventMatchTree: public MatchTree<EventMessage*>, public HasReturnvaluesIF {
public:
	EventMatchTree(StorageManagerIF* storageBackend, bool invertedMatch);
	virtual ~EventMatchTree();
	ReturnValue_t addMatch(EventId_t idFrom = 0, EventId_t idTo = 0, bool idInverted = false,
			object_id_t reporterFrom = 0, object_id_t reporterTo = 0,
			bool reporterInverted = false);
	ReturnValue_t removeMatch(EventId_t idFrom = 0, EventId_t idTo = 0, bool idInverted = false,
			object_id_t reporterFrom = 0, object_id_t reporterTo = 0,
			bool reporterInverted = false);
	bool match(EventMessage* number);
protected:
	ReturnValue_t cleanUpElement(iterator position);
private:
	PlacementFactory factory;
	bool invertedMatch;
	template<typename VALUE_T, typename INSERTION_T>
	ReturnValue_t findOrInsertRangeMatcher(iterator start, VALUE_T idFrom,
			VALUE_T idTo, bool inverted, iterator* lastTest);

	template<typename VALUE_T, typename INSERTION_T>
	iterator findRangeMatcher(iterator start, VALUE_T idFrom, VALUE_T idTo,
			bool inverted);

};

#endif /* FRAMEWORK_EVENTS_EVENTMATCHING_EVENTMATCHTREE_H_ */
