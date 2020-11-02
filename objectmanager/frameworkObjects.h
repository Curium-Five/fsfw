#ifndef FSFW_OBJECTMANAGER_FRAMEWORKOBJECTS_H_
#define FSFW_OBJECTMANAGER_FRAMEWORKOBJECTS_H_

namespace objects {
enum framework_objects {
	// Default verification reporter.
	PUS_SERVICE_1_VERIFICATION = 0x53000001,
	PUS_SERVICE_2_DEVICE_ACCESS = 0x53000002,
	PUS_SERVICE_5_EVENT_REPORTING = 0x53000005,
	PUS_SERVICE_8_FUNCTION_MGMT = 0x53000008,
	PUS_SERVICE_9_TIME_MGMT = 0x53000009,
	PUS_SERVICE_17_TEST = 0x53000017,
	PUS_SERVICE_200_MODE_MGMT = 0x53000200,

	//Generic IDs for IPC, modes, health, events
	HEALTH_TABLE = 0x53010000,
//	MODE_STORE = 0x53010100,
	EVENT_MANAGER = 0x53030000,
	INTERNAL_ERROR_REPORTER = 0x53040000,
	IPC_STORE = 0x534f0300,
	//IDs for PUS Packet Communication
	TC_STORE = 0x534f0100,
	TM_STORE = 0x534f0200,

	NO_OBJECT = 0xFFFFFFFF
};
}



#endif /* FSFW_OBJECTMANAGER_FRAMEWORKOBJECTS_H_ */
