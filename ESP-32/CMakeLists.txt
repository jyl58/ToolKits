#check the sdkconfig file
exec_program(bash ARGS $ENV{ADF_PATH}/../../config/update_sdkconfig.sh)
string(TIMESTAMP BUILD_TIME)
message("##build time: ${BUILD_TIME}" )
add_definitions(-DAIBOX_VERSION="${BUILD_TIME}")

#debug switch
OPTION(AIBOX_DEBUG "Debug mode" OFF)
if (AIBOX_DEBUG)
	#print the dns message
	add_definitions(-DDNS_DEBUG=0x80)
endif()

include($ENV{ADF_PATH}/CMakeLists.txt)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(EXTRA_COMPONENT_DIRS ${EXTRA_COMPONENT_DIRS}  $ENV{ADF_PATH}/../../components)