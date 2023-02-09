# Check the OS_FSFW variable
if(FSFW_OSAL MATCHES "freertos")
  add_subdirectory(freertos)
  set(FSFW_OSAL_FREERTOS 1)
elseif(FSFW_OSAL MATCHES "rtems")
  add_subdirectory(rtems)
  set(FSFW_OSAL_RTEMS 1)
elseif(FSFW_OSAL MATCHES "linux")
  add_subdirectory(linux)
  set(FSFW_OSAL_LINUX 1)
elseif(FSFW_OSAL MATCHES "host")
  add_subdirectory(host)
  if(WIN32)
    add_subdirectory(windows)
  elseif(UNIX)
    # We still need to pull in some Linux specific sources
    target_sources(${LIB_FSFW_NAME} PUBLIC linux/tcpipHelpers.cpp)
  endif()
  set(FSFW_OSAL_HOST 1)
else()
  message(
    WARNING
      "${MSG_PREFIX} The FSFW_OSAL variable was not set. Assuming host OS..")

  # Not set. Assumuing this is a host build, try to determine host OS
  if(WIN32)
    add_subdirectory(host)
    add_subdirectory(windows)
    set(FSFW_OSAL_HOST 1)
  elseif(UNIX)
    add_subdirectory(linux)
    set(FSFW_OSAL_LINUX 1)
  else()
    # MacOS or other OSes have not been tested yet / are not supported.
    message(FATAL_ERROR "The host OS could not be determined! Aborting.")
  endif()

endif()

add_subdirectory(common)

configure_file(osal.h.in ${CMAKE_BINARY_DIR}/fsfw/osal/osal.h)
