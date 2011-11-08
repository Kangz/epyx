# Run cmake . to build Makefile

cmake_minimum_required(VERSION 2.6)

# Project name
project(Epyx)

# Test executables
add_executable(
    bin/test-server
    src/core/address.cpp
    src/core/exception.cpp
    src/core/thread.cpp
    src/server/server.cpp
    src/test-server.cpp
)

target_link_libraries(
    bin/test-server
    pthread
)
