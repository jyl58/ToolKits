#####################################
# AiBox Console CMakeLists.txt file
# author: 	jiyingliang
# email : 	jiyingliang@hisense.com
# date  :	2020.9.2
#####################################
cmake_minimum_required(VERSION 3.6)
project(AiBoxConsole)

OPTION(TEST  "Test compile" ON)

include_directories(
inc/
)
FILE (GLOB SRC_LISTS 
src/*.cpp
)
add_library(${PROJECT_NAME} SHARED ${SRC_LISTS})

if (TEST)
	link_directories(
	${PROJECT_SOURCE_DIR}/build
	)
	message("build test app")
	add_executable(test_console test/main.cpp)
	target_link_libraries(test_console
		${PROJECT_NAME}
		pthread
	)
endif()