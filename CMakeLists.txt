# Run cmake . to build Makefile

cmake_minimum_required(VERSION 2.6)

# Project name
project(Epyx)

# Test executables
add_executable(
    test-server
    src/address.cpp
    src/exception.cpp
    src/server.cpp
    src/test-server.cpp
    src/thread.cpp
)

target_link_libraries(
    test-server
    pthread
)
