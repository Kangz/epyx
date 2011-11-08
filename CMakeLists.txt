# Run cmake . to build Makefile

cmake_minimum_required(VERSION 2.6)

# Project name
project(Epyx)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

# Test executables
add_executable(
    test-server
    src/core/address.cpp
    src/core/exception.cpp
    src/core/thread.cpp
    src/server/server.cpp
    src/server/server-link.cpp
    src/server/server-listener.cpp
    src/test-server.cpp
)

target_link_libraries(
    test-server
    pthread
)
