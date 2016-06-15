/*
 * SystemObject.cpp
 *
 *  Created on: 07.11.2012
 *      Author: mohr
 */

#include <framework/events/EventManagerIF.h>
#include <framework/objectmanager/ObjectManager.h>
#include <framework/objectmanager/SystemObject.h>

SystemObject::SystemObject(object_id_t setObjectId, bool doRegister) :
		objectId(setObjectId), registered(doRegister) {
	if (registered) {
		objectManager->insert(objectId, this);
	}
}

SystemObject::~SystemObject() {
	if (registered) {
		objectManager->remove(objectId);
	}
}

const object_id_t SystemObject::getObjectId() const {
	return objectId;
}

void SystemObject::triggerEvent(Event event, uint32_t parameter1,
		uint32_t parameter2) {
	EventManagerIF::triggerEvent(objectId, event, parameter1, parameter2);
}

ReturnValue_t SystemObject::initialize() {
	return HasReturnvaluesIF::RETURN_OK;
}

ReturnValue_t SystemObject::checkObjectConnections() {
	return HasReturnvaluesIF::RETURN_OK;
}

void SystemObject::forwardEvent(Event event, uint32_t parameter1, uint32_t parameter2) const {
	EventManagerIF::triggerEvent(objectId, event, parameter1, parameter2);
}
