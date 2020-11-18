#ifndef FSFW_THERMAL_THERMALCOMPONENTCORE_H_
#define FSFW_THERMAL_THERMALCOMPONENTCORE_H_

#include "ThermalMonitorReporter.h"
#include "ThermalComponentIF.h"
#include "AbstractTemperatureSensor.h"
#include "ThermalModule.h"

#include "../datapoolglob/GlobalDataSet.h"
#include "../datapoolglob/GlobalPoolVariable.h"

/**
 * @brief
 * @details
 */
class ThermalComponentCore: public ThermalComponentIF {
public:
	struct Parameters {
		float lowerOpLimit;
		float upperOpLimit;
		float heaterOn;
		float hysteresis;
		float heaterSwitchoff;
	};

	static const uint16_t COMPONENT_TEMP_CONFIRMATION = 5;

	/**
	 * Some documentation
	 * @param reportingObjectId
	 * @param domainId
	 * @param temperaturePoolId
	 * @param targetStatePoolId
	 * @param currentStatePoolId
	 * @param requestPoolId
	 * @param dataSet
	 * @param parameters
	 * @param initialTargetState
	 */
	ThermalComponentCore(object_id_t reportingObjectId, uint8_t domainId,
	        uint32_t temperaturePoolId, uint32_t targetStatePoolId,
	        uint32_t currentStatePoolId, uint32_t requestPoolId,
	        GlobDataSet *dataSet, Parameters parameters,
			StateRequest initialTargetState =
					ThermalComponentIF::STATE_REQUEST_OPERATIONAL);

	void addSensor(AbstractTemperatureSensor* firstRedundantSensor);
	void addFirstRedundantSensor(
	        AbstractTemperatureSensor* firstRedundantSensor);
    void addSecondRedundantSensor(
            AbstractTemperatureSensor* secondRedundantSensor);
    void addThermalModule(ThermalModule* thermalModule, Priority priority);

    void setPriority(Priority priority);

	virtual ~ThermalComponentCore();

	virtual HeaterRequest performOperation(uint8_t opCode);

	void markStateIgnored();

	object_id_t getObjectId();

	uint8_t getDomainId() const;

	virtual float getLowerOpLimit();

	ReturnValue_t setTargetState(int8_t newState);

	virtual void setOutputInvalid();

	virtual ReturnValue_t getParameter(uint8_t domainId, uint16_t parameterId,
			ParameterWrapper *parameterWrapper,
			const ParameterWrapper *newValues, uint16_t startAtIndex);

protected:

	AbstractTemperatureSensor *sensor = nullptr;
	AbstractTemperatureSensor *firstRedundantSensor = nullptr;
	AbstractTemperatureSensor *secondRedundantSensor = nullptr;
	ThermalModuleIF *thermalModule = nullptr;

	gp_float_t temperature;
	gp_int8_t targetState;
	gp_int8_t currentState;
	gp_uint8_t heaterRequest;

	bool isHeating = false;

	bool isSafeComponent = false;

	float minTemp = 999;

	float maxTemp = AbstractTemperatureSensor::ZERO_KELVIN_C;

	Parameters parameters;

	ThermalMonitorReporter temperatureMonitor;

	const uint8_t domainId;

	virtual float getTemperature();
	virtual State getState(float temperature, Parameters parameters,
			int8_t targetState);

	virtual void checkLimits(State state);

	virtual HeaterRequest getHeaterRequest(int8_t targetState,
			float temperature, Parameters parameters);

	virtual State getIgnoredState(int8_t state);

	void updateMinMaxTemp();

	virtual Parameters getParameters();
};

#endif /* FSFW_THERMAL_THERMALCOMPONENT_CORE_H_ */
