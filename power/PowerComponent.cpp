#include "PowerComponent.h"
#include "../serialize/SerializeAdapter.h"


PowerComponent::PowerComponent(): switchId1(0xFF), switchId2(0xFF),
        doIHaveTwoSwitches(false) {
}

PowerComponent::PowerComponent(object_id_t setId, uint8_t moduleId, float min,
        float max, uint8_t switchId1, bool twoSwitches, uint8_t switchId2) :
		deviceObjectId(setId), switchId1(switchId1), switchId2(switchId2),
		doIHaveTwoSwitches(twoSwitches), minVoltage(min), maxVoltage(max),
		moduleId(moduleId) {
}

ReturnValue_t PowerComponent::serialize(uint8_t** buffer, size_t* size,
		size_t maxSize, Endianness streamEndianness) const {
	ReturnValue_t result = SerializeAdapter::serialize(&minVoltage, buffer,
			size, maxSize, streamEndianness);
	if (result != HasReturnvaluesIF::RETURN_OK) {
		return result;
	}
	return SerializeAdapter::serialize(&maxVoltage, buffer, size, maxSize,
			streamEndianness);
}

size_t PowerComponent::getSerializedSize() const {
	return sizeof(minVoltage) + sizeof(maxVoltage);
}

object_id_t PowerComponent::getDeviceObjectId() {
	return deviceObjectId;
}

uint8_t PowerComponent::getSwitchId1() {
	return switchId1;
}

uint8_t PowerComponent::getSwitchId2() {
	return switchId2;
}

bool PowerComponent::hasTwoSwitches() {
	return doIHaveTwoSwitches;
}

float PowerComponent::getMin() {
	return minVoltage;
}

float PowerComponent::getMax() {
	return maxVoltage;
}

ReturnValue_t PowerComponent::deSerialize(const uint8_t** buffer, size_t* size,
        Endianness streamEndianness) {
	ReturnValue_t result = SerializeAdapter::deSerialize(&minVoltage, buffer,
			size, streamEndianness);
	if (result != HasReturnvaluesIF::RETURN_OK) {
		return result;
	}
	return SerializeAdapter::deSerialize(&maxVoltage, buffer, size, streamEndianness);
}

ReturnValue_t PowerComponent::getParameter(uint8_t domainId, uint8_t uniqueId,
        ParameterWrapper* parameterWrapper, const ParameterWrapper* newValues,
        uint16_t startAtIndex) {
	if (domainId != moduleId) {
		return INVALID_DOMAIN_ID;
	}
	switch (uniqueId) {
	case 0:
		parameterWrapper->set<>(minVoltage);
		break;
	case 1:
		parameterWrapper->set<>(maxVoltage);
		break;
	default:
		return INVALID_IDENTIFIER_ID;
	}
	return HasReturnvaluesIF::RETURN_OK;
}
