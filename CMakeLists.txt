cmake_minimum_required(VERSION 2.8.9)

add_definitions("-std=c99")

project(monty_hall C)

include_directories(${PROJECT_SOURCE_DIR}/include)

set(SOURCES ${PROJECT_SOURCE_DIR}/src/monty_hall.c
	