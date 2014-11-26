cmake_minimum_required(VERSION 2.8.9)

add_definitions("-m32 -std=c99 -Wall")

IF(CMAKE_BUILD_TYPE MATCHES DEBUG)
	#add extra warnings
	add_definitions(-g -Wextra -Wstrict-prototypes -Wundef -Wcast-qual -Wconversion -Wformat=2 -Wshadow -ftrapv -Wuninitialized -Winit-self -Wcast-align -Wwrite-strings)
    ENDIF(CMAKE_BUILD_TYPE MATCHES DEBUG)

project(monty_hall C)

include_directories(${PROJECT_SOURCE_DIR}/include)

set(SOURCES ${PROJECT_SOURCE_DIR}/src/monty_hall.c
	${PROJECT_SOURCE_DIR}/src/rand.c)

set(MONTY_HALL_EXE monty_hall_simulator)

add_executable(${MONTY_HALL_EXE} ${SOURCES})

#force 32 bit compilation
set_target_properties(${MONTY_HALL_EXE} PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
