cmake_minimum_required(VERSION 3.18)
project(ElevatorControl)

set(CMAKE_CXX_STANDARD 20)
set(TARGET ElevatorControl)

# Find libcurl
find_package(CURL REQUIRED)

list(APPEND SOURCES
	src/xserial.cpp
    src/xserial.hpp
	src/elevator_control.cpp
	src/elevator_control.h
	src/input_reader.cpp
	src/input_reader.h
	src/json_reader.h
	src/json_reader.cpp
	src/json.h
	src/json.cpp
	src/json_builder.cpp
	src/json_builder.h
	src/magic_enum.hpp
	src/main.cpp
	src/network_client.cpp
	src/network_client.h) 

if(EXTERNAL_DISPLAY)
	list(PREPEND SOURCES 
	src/display.cpp
	src/display.h
	src/gpio.h
	src/gpio.cpp)
endif()

add_executable(${TARGET} ${SOURCES})
target_link_libraries(${TARGET} PRIVATE CURL::libcurl)   

if(EXTERNAL_DISPLAY)
	add_subdirectory(../WiringRP ${CMAKE_BINARY_DIR}/wrp)
	target_link_libraries(${TARGET} PRIVATE wiringrp)
	add_compile_definitions(EXTERNAL_DISPLAY=1)
else()
	add_compile_definitions(EXTERNAL_DISPLAY=0)
endif()
