#ifndef FSFW_OBJECTMANAGER_FRAMEWORKOBJECTS_H_
#define FSFW_OBJECTMANAGER_FRAMEWORKOBJECTS_H_

#include "SystemObjectIF.h"

// The objects will be instantiated in the ID order
namespace objects {
enum framework_objects : object_id_t {
  FSFW_OBJECTS_START = 0x53000000,
  // Default verification reporter.
  PUS_SERVICE_1_VERIFICATION = 0x53000001,
  PUS_SERVICE_2_DEVICE_ACCESS = 0x53000002,
  PUS_SERVICE_3_HOUSEKEEPING = 0x53000003,
  PUS_SERVICE_5_EVENT_REPORTING = 0x53000005,
  PUS_SERVICE_8_FUNCTION_MGMT = 0x53000008,
  PUS_SERVICE_9_TIME_MGMT = 0x53000009,
  PUS_SERVICE_11_TC_SCHEDULER = 0x53000011,
  PUS_SERVICE_17_TEST = 0x53000017,
  PUS_SERVICE_20_PARAMETERS = 0x53000020,
  PUS_SERVICE_200_MODE_MGMT = 0x53000200,
  PUS_SERVICE_201_HEALTH = 0x53000201,

  /* CFDP Distributer */
  CFDP_PACKET_DISTRIBUTOR = 0x53001000,

  // Generic IDs for IPC, modes, health, events
  HEALTH_TABLE = 0x53010000,
  //	MODE_STORE = 0x53010100,
  EVENT_MANAGER = 0x53030000,
  INTERNAL_ERROR_REPORTER = 0x53040000,
  IPC_STORE = 0x534f0300,
  // IDs for PUS Packet Communication
  TC_STORE = 0x534f0100,
  TM_STORE = 0x534f0200,
  TIME_STAMPER = 0x53500010,
  VERIFICATION_REPORTER = 0x53500020,

  FSFW_OBJECTS_END = 0x53ffffff,
  NO_OBJECT = 0xFFFFFFFF
};
}

#endif /* FSFW_OBJECTMANAGER_FRAMEWORKOBJECTS_H_ */
