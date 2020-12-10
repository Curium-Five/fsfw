#ifndef FSFW_POWER_POWERCOMPONENT_H_
#define FSFW_POWER_POWERCOMPONENT_H_

#include "PowerComponentIF.h"

#include "../objectmanager/frameworkObjects.h"
#include "../objectmanager/SystemObjectIF.h"


class PowerComponent: public PowerComponentIF {
public:
	PowerComponent(object_id_t setId, uint8_t moduleId, float min, float max,
	        uint8_t switchId1, bool twoSwitches = false,
	        uint8_t switchId2 = 0xFF);

	virtual object_id_t getDeviceObjectId();

	virtual uint8_t getSwitchId1();
	virtual uint8_t getSwitchId2();

	bool hasTwoSwitches();

	float getMin();
	float getMax();

	ReturnValue_t serialize(uint8_t** buffer, size_t* size,
			size_t maxSize, Endianness streamEndianness) const override;

	size_t getSerializedSize() const override;

	ReturnValue_t deSerialize(const uint8_t** buffer, size_t* size,
			Endianness streamEndianness) override;

	ReturnValue_t getParameter(uint8_t domainId, uint16_t parameterId,
				ParameterWrapper *parameterWrapper,
				const ParameterWrapper *newValues, uint16_t startAtIndex);
private:
	const object_id_t deviceObjectId = objects::NO_OBJECT;
	const uint8_t switchId1;
	const uint8_t switchId2;

	const bool doIHaveTwoSwitches;

	float min = 0.0;
	float max = 0.0;

	uint8_t moduleId = 0;

	PowerComponent();
};

#endif /* FSFW_POWER_POWERCOMPONENT_H_ */
