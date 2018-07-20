# This file needs FRAMEWORK_PATH and API set correctly
# Valid API settings: rtems, linux, freeRTOS

CXXSRC += $(wildcard $(FRAMEWORK_PATH)/action/*.cpp)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/container/*.cpp)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/contrib/sgp4/*.cpp)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/controller/*.cpp)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/coordinates/*.cpp)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/datalinklayer/*.cpp)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/datapool/*.cpp)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/devicehandlers/*.cpp)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/events/*.cpp)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/events/eventmatching/*.cpp)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/fdir/*.cpp)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/framework.mk/*.cpp)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/globalfunctions/*.cpp)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/globalfunctions/matching/*.cpp)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/globalfunctions/math/*.cpp)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/health/*.cpp)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/internalError/*.cpp)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/ipc/*.cpp)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/memory/*.cpp)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/modes/*.cpp)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/monitoring/*.cpp)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/objectmanager/*.cpp)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/osal/*.cpp)

# select the OS
ifeq ($(OS),rtems)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/osal/rtems/*.cpp)
else ifeq ($(OS),linux)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/osal/linux/*.cpp)
else ifeq ($(OS),freeRTOS)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/osal/FreeRTOS/*.cpp)
else
$(error invalid OS specified, valid OS are rtems, linux, freeRTOS)
endif

CXXSRC += $(wildcard $(FRAMEWORK_PATH)/parameters/*.cpp)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/power/*.cpp)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/returnvalues/*.cpp)

# easier without it for now
#CXXSRC += $(wildcard $(FRAMEWORK_PATH)/rmap/*.cpp)

CXXSRC += $(wildcard $(FRAMEWORK_PATH)/serialize/*.cpp)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/serviceinterface/*.cpp)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/storagemanager/*.cpp)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/subsystem/*.cpp)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/subsystem/modes/*.cpp)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/tasks/*.cpp)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/tcdistribution/*.cpp)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/thermal/*.cpp)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/timemanager/*.cpp)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/tmstorage/*.cpp)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/tmtcpacket/*.cpp)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/tmtcpacket/packetmatcher/*.cpp)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/tmtcpacket/pus/*.cpp)
CXXSRC += $(wildcard $(FRAMEWORK_PATH)/tmtcservices/*.cpp)