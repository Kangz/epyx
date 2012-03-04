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
    localn2np_sources
    src/localn2np/*
    src/n2np/*
)

file(
    GLOB_RECURSE
    natpunching_sources
    src/natpunching/*
)

file(
    GLOB_RECURSE
    net_sources
    src/net/*
)

file(
    GLOB_RECURSE
    parser_sources
    src/parser/*
)

# Test executables
add_executable(
    test-thread
    ${core_sources}
    src/test-thread.cpp
)
target_link_libraries(
    test-thread
    pthread
)

add_executable(
    test-server
    ${core_sources}
    ${net_sources}
    src/test-server.cpp
)
target_link_libraries(
    test-server
    pthread
)

add_executable(
    test-parser
    ${core_sources}
    ${parser_sources}
    src/test-parser.cpp
)
target_link_libraries(
    test-parser
    pthread
)

add_executable(
    test-natpunching
    ${core_sources}
    ${net_sources}
    ${natpunching_sources}
    ${tinyxml_sources}
)

target_link_libraries(
    test-natpunching
    pthread
)

add_executable(
    test-n2np
    ${core_sources}
    ${net_sources}
    ${localn2np_sources}
    src/test-n2np.cpp
)
target_link_libraries(
    test-n2np
    pthread
)
