# Run cmake . to build Makefile

cmake_minimum_required(VERSION 2.6)

# Project name
project(Epyx)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

SET(CMAKE_CXX_FLAGS "-g -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable")

# Make some sets of files

file(
    GLOB_RECURSE
    tinyxml_sources
    include/tinyxml/*
)

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
file(
    GLOB_RECURSE
    natpunching_sources
    src/natpunching/*
)

file(
    GLOB_RECURSE
    localn2np_sources
    src/localn2np/*
    src/n2np/*
)

# Test executables
#add_executable(
#    test-server
#    ${core_sources}
#    ${server_sources}
#    src/test-server.cpp
#)
#target_link_libraries(
#    test-server
#    pthread
#)

add_executable(
    test-thread
    ${core_sources}
    ${natpunching_sources}
    ${tinyxml_sources}
    src/test-thread.cpp
)
target_link_libraries(
    test-thread
    pthread
)

#add_executable(
#    test-n2np
#    ${core_sources}
#    ${localn2np_sources}
#    src/test-n2np.cpp
#)
#target_link_libraries(
#    test-n2np
#    pthread
#)
