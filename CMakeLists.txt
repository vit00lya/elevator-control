cmake_minimum_required(VERSION 3.10)
project(ElevatorControl)

set(CMAKE_CXX_STANDARD 20)
set(TARGET ComPortTools)

# Поиск необходимых пакетов
#find_package(PostgreSQL REQUIRED)

list(APPEND SOURCES
	src/xserial.cpp
        src/xserial.hpp
	src/ElevatorControl.cpp)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/libpqxx/include)

add_executable(${TARGET} ${SOURCES})
target_include_directories(${TARGET} PRIVATE lib/)

target_link_libraries(${TARGET} PRIVATE pqxx)
