cmake_minimum_required(VERSION 3.18)
project(ElevatorControl)

set(CMAKE_CXX_STANDARD 20)
set(TARGET ElevatorControl)

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
	src/main.cpp)

add_executable(${TARGET} ${SOURCES})
