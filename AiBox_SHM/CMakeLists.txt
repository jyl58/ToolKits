####################################
# AiBox shared memory CMakeLists.txt file
# author: 	jiyingliang
# email : 	jiyingliang@hisense.com
# date  :	2020.8.26
#####################################
cmake_minimum_required(VERSION 3.10)
project(AiBoxSHM)

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
	lib/boost/include/
)
#add src file
FILE (GLOB SRC_LISTS 
	src/*.cpp
)
link_directories(
${PROJECT_SOURCE_DIR}/build
${PROJECT_SOURCE_DIR}/test
)
add_library(${PROJECT_NAME} SHARED ${SRC_LISTS})
target_link_libraries(${PROJECT_NAME}  PRIVATE pthread rt)

add_executable(write_shm test/write_main.cpp)
target_link_libraries(write_shm  PRIVATE ${PROJECT_NAME} glib-2.0)

add_executable(read_shm test/read_main.cpp)
target_link_libraries(read_shm  PRIVATE ${PROJECT_NAME})

add_executable(clear_shm test/clear_main.cpp)
target_link_libraries(clear_shm  PRIVATE ${PROJECT_NAME})