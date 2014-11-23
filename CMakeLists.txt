cmake_minimum_required(VERSION 2.8.9)

add_definitions("-std=c99")

project(monty_hall C)

include_directories(${PROJECT_SOURCE_DIR}/include)

set(SOURCES ${PROJECT_SOURCE_DIR}/src/monty_hall.c
	${PROJECT_SOURCE_DIR}/src/isaac64.c)

set(MONTY_HALL_EXE monty_hall_simulator)

add_executable(${MONTY_HALL_EXE} ${SOURCES})