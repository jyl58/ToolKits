####################################
# cooper system CMakeLists.txt file
# author: 	jiyingliang
# email : 	jiyingliang@hisense.com
# date  :	2020.4.3
#####################################
cmake_minimum_required(VERSION 3.10)
project(cooper-system)

OPTION(DEBUG "Debug mode" ON)

#add the debug symbl param
if (DEBUG)
	add_definitions(-DDEBUG)
	message("make for debug mode.")
endif()

add_compile_options(-std=c++14 -Wno-psabi)
string(TIMESTAMP BUILD_TIME)
add_definitions(-DBUILD_VERSION="${BUILD_TIME}")
#set the compiler
set(CMAKE_C_COMPILER /usr/bin/arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER /usr/bin/arm-linux-gnueabihf-g++)
#add include file
include_directories(
	inc/
	lib/lcm/
 	lib/lcm/inc/
 	lib/lcm/inc/lcmtypes/
	lib/json/inc/
	lib/mosquitto/inc/
)
#add src file
FILE (GLOB SRC_LISTS 
	src/*.cpp
	lib/json/src/*.c
)

add_library(${PROJECT_NAME} SHARED ${SRC_LISTS})
target_link_libraries(${PROJECT_NAME} PRIVATE 
${CMAKE_SOURCE_DIR}/lib/lcm/lib/liblcm.a
${CMAKE_SOURCE_DIR}/lib/mosquitto/lib/libmosquitto.so.1
)

link_directories(${PROJECT_SOURCE_DIR}/test)
add_executable(test_lcm test/main.cpp)
target_link_libraries(test_lcm  PRIVATE  cooper-system glib-2.0)