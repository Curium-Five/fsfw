#ifndef POWERCOMPONENT_H_
#define POWERCOMPONENT_H_

#include <framework/objectmanager/SystemObjectIF.h>
#include <framework/power/PowerComponentIF.h>

class PowerComponent: public PowerComponentIF {
public:
	PowerComponent(object_id_t setId, uint8_t moduleId, float min, float max, uint8_t switchId1,
			bool twoSwitches = false, uint8_t switchId2 = 0xFF);

	virtual object_id_t getDeviceObjectId();

	virtual uint8_t getSwitchId1();
	virtual uint8_t getSwitchId2();

	bool hasTwoSwitches();

	float getMin();
	float getMax();

	ReturnValue_t serialize(uint8_t** buffer, size_t* size,
			const size_t max_size, bool bigEndian) const;

	uint32_t getSerializedSize() const;

	ReturnValue_t deSerialize(const uint8_t** buffer, int32_t* size,
			bool bigEndian);

	ReturnValue_t getParameter(uint8_t domainId, uint16_t parameterId,
				ParameterWrapper *parameterWrapper,
				const ParameterWrapper *newValues, uint16_t startAtIndex);
private:
	const object_id_t deviceObjectId;
	const uint8_t switchId1;
	const uint8_t switchId2;

	const bool doIHaveTwoSwitches;

	float min;
	float max;

	uint8_t moduleId;

	PowerComponent();
};

#endif /* POWERCOMPONENT_H_ */
