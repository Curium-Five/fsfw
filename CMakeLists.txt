cmake_minimum_required(VERSION 3.13)

option(FSFW_GENERATE_SECTIONS
    "Generate function and data sections. Required to remove unused code" ON
)

if(FSFW_GENERATE_SECTIONS)
    option(FSFW_REMOVE_UNUSED_CODE "Remove unused code" ON)
endif()

option(FSFW_WARNING_SHADOW_LOCAL_GCC "Enable -Wshadow=local warning in GCC" ON)
# Options to exclude parts of the FSFW from compilation.
option(FSFW_USE_RMAP "Compile with RMAP" ON)
option(FSFW_USE_DATALINKLAYER "Compile with Data Link Layer" ON)

set(LIB_FSFW_NAME fsfw)
add_library(${LIB_FSFW_NAME})

set_property(CACHE OS_FSFW PROPERTY STRINGS host linux rtems freertos)

if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED True)
elseif(${CMAKE_CXX_STANDARD} LESS 11)
    message(FATAL_ERROR "Compiling the FSFW requires a minimum of C++11 support")
endif()

if(NOT OS_FSFW)
	message(STATUS "No OS for FSFW via OS_FSFW set. Assuming host OS")
	# Assume host OS and autodetermine from OS_FSFW
	if(UNIX)
		set(OS_FSFW  "linux" 
			CACHE STRING 
			"OS abstraction layer used in the FSFW"
		)
	elseif(WIN32)
		set(OS_FSFW  "host" 
			CACHE STRING "OS abstraction layer used in the FSFW"
		)
	endif()

endif()

set(FSFW_OSAL_DEFINITION FSFW_HOST)

if(${OS_FSFW} STREQUAL host)
	set(OS_FSFW_NAME "Host")
elseif(${OS_FSFW} STREQUAL linux)
	set(OS_FSFW_NAME "Linux")
	set(FSFW_OSAL_DEFINITION FSFW_LINUX)
elseif(${OS_FSFW} STREQUAL  freertos)
	set(OS_FSFW_NAME "FreeRTOS")
	set(FSFW_OSAL_DEFINITION FSFW_FREERTOS)
	target_link_libraries(${LIB_FSFW_NAME} PRIVATE
        ${LIB_OS_NAME}
	)
elseif(${OS_FSFW} STREQUAL rtems)
	set(OS_FSFW_NAME "RTEMS")
    set(FSFW_OSAL_DEFINITION FSFW_RTEMS)
else()
	message(WARNING 
		"Invalid operating system for FSFW specified! Setting to host.."
	)
	set(OS_FSFW_NAME "Host")
	set(OS_FSFW "host")
endif()

target_compile_definitions(${LIB_FSFW_NAME} PRIVATE
    ${FSFW_OSAL_DEFINITION}
)

target_compile_definitions(${LIB_FSFW_NAME} INTERFACE
    ${FSFW_OSAL_DEFINITION}
)

message(STATUS "Compiling FSFW for the ${OS_FSFW_NAME} operating system.")

add_subdirectory(action)
add_subdirectory(container)
add_subdirectory(controller)
add_subdirectory(coordinates)

if(FSFW_USE_DATALINKLAYER)
	add_subdirectory(datalinklayer)
endif()

add_subdirectory(datapool)
add_subdirectory(datapoollocal)
add_subdirectory(housekeeping)
add_subdirectory(devicehandlers)
add_subdirectory(events)
add_subdirectory(fdir)
add_subdirectory(globalfunctions)
add_subdirectory(health)
add_subdirectory(internalError)
add_subdirectory(ipc)
add_subdirectory(memory)
add_subdirectory(modes)
add_subdirectory(monitoring)
add_subdirectory(objectmanager)
add_subdirectory(osal)
add_subdirectory(parameters)
add_subdirectory(power)
add_subdirectory(pus)

if(FSFW_USE_RMAP)
	add_subdirectory(rmap)
endif()

add_subdirectory(serialize)
add_subdirectory(serviceinterface)
add_subdirectory(storagemanager)
add_subdirectory(subsystem)
add_subdirectory(tasks)
add_subdirectory(tcdistribution)
add_subdirectory(thermal)
add_subdirectory(timemanager)
add_subdirectory(tmstorage)
add_subdirectory(tmtcpacket)
add_subdirectory(tmtcservices)
add_subdirectory(unittest)

# The project CMakeLists file has to set the FSFW_CONFIG_PATH and add it.
# If this is not given, we include the default configuration and emit a warning.
if(NOT FSFW_CONFIG_PATH)
	message(WARNING "Flight Software Framework configuration path not set!")
	message(WARNING "Setting default configuration!")
	add_subdirectory(defaultcfg/fsfwconfig)
endif()

# FSFW might be part of a possibly complicated folder structure, so we 
# extract the absolute path of the fsfwconfig folder.
if(IS_ABSOLUTE ${FSFW_CONFIG_PATH})
	set(FSFW_CONFIG_PATH_ABSOLUTE ${FSFW_CONFIG_PATH})
else()
	get_filename_component(FSFW_CONFIG_PATH_ABSOLUTE
		${FSFW_CONFIG_PATH} REALPATH BASE_DIR ${CMAKE_SOURCE_DIR}
	)
endif()

foreach(INCLUDE_PATH ${FSFW_ADDITIONAL_INC_PATHS})
    if(IS_ABSOLUTE ${INCLUDE_PATH})
        set(CURR_ABS_INC_PATH "${INCLUDE_PATH}")
    else()
        get_filename_component(CURR_ABS_INC_PATH
            ${INCLUDE_PATH} REALPATH BASE_DIR ${CMAKE_SOURCE_DIR})
    endif()

    if(CMAKE_VERBOSE)
        message(STATUS "FSFW include path: ${CURR_ABS_INC_PATH}")
    endif()

    list(APPEND FSFW_ADD_INC_PATHS_ABS ${CURR_ABS_INC_PATH})
endforeach()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(NOT DEFINED FSFW_WARNING_FLAGS)
	   set(FSFW_WARNING_FLAGS
		  -Wall
		  -Wextra
		  -Wimplicit-fallthrough=1
		  -Wno-unused-parameter
		  -Wno-psabi
        )
    endif()

    if(FSFW_GENERATE_SECTIONS)
        target_compile_options(${LIB_FSFW_NAME} PRIVATE
            "-ffunction-sections"
            "-fdata-sections"
        )
    endif()

    if(FSFW_REMOVE_UNUSED_CODE)
        target_link_options(${LIB_FSFW_NAME} PRIVATE
            "Wl,--gc-sections"
        )
    endif()
  
    if(FSFW_WARNING_SHADOW_LOCAL_GCC)
        list(APPEND WARNING_FLAGS "-Wshadow=local")
    endif()
	
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	set(COMPILER_FLAGS "/permissive-")
endif()

# Required include paths to compile the FSFW
target_include_directories(${LIB_FSFW_NAME} INTERFACE 
	${CMAKE_SOURCE_DIR}
	${FSFW_CONFIG_PATH_ABSOLUTE}
	${FSFW_ADD_INC_PATHS_ABS}
)

# Includes path required to compile FSFW itself as well
# We assume that the fsfwconfig folder uses include relative to the project
# root here!
target_include_directories(${LIB_FSFW_NAME} PRIVATE 
	${CMAKE_SOURCE_DIR}
	${FSFW_CONFIG_PATH_ABSOLUTE}
	${FSFW_ADD_INC_PATHS_ABS}
)

target_compile_options(${LIB_FSFW_NAME} PRIVATE 
	${FSFW_WARNING_FLAGS}
	${COMPILER_FLAGS}
)

target_link_libraries(${LIB_FSFW_NAME} PRIVATE
    ${FSFW_ADDITIONAL_LINK_LIBS}
)