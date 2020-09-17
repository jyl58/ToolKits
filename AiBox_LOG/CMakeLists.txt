####################################
# AiBox LOG CMakeLists.txt file
# author: 	jiyingliang
# email : 	jiyingliang@hisense.com
# date  :	2020.8.26
#####################################
cmake_minimum_required(VERSION 3.10)
project(AiBoxLOG)
add_compile_options(-std=c++14 -Wno-psabi -Wl,-z,multidefs)

include_directories(
	inc/
)
FILE (GLOB SRC_LISTS 
	src/*.cpp
)
add_executable(log_test test/main.cpp test/src.cpp ${SRC_LISTS})