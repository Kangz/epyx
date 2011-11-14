# Run cmake . to build Makefile

cmake_minimum_required(VERSION 2.6)

# Project name
project(Epyx)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

# Make some sets of files

file(
	GLOB_RECURSE
	core_sources
	src/core/*
)

file(
	GLOB_RECURSE
	server_sources
	src/server/*
)

# Test executables
add_executable(
    test-server
    ${core_sources}
    ${server_sources}
    src/test-server.cpp
)
target_link_libraries(
    test-server
    pthread
)
