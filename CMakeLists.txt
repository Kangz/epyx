# Run cmake . to build Makefile

cmake_minimum_required(VERSION 2.6)

# Project name
project(Epyx)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

SET(CMAKE_CXX_FLAGS "-g -Wall -Wextra -pedantic -std=c++11 -Wno-unused-parameter -Wno-unused-variable")

# Use Boost
find_package(Boost 1.40.0 COMPONENTS thread)
link_directories(${Boost_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIRS})

# Make some sets of files from directories
file(GLOB_RECURSE tinyxml_sources include/tinyxml/*)
file(GLOB_RECURSE core_sources src/core/*)
file(GLOB_RECURSE n2np_sources src/n2np/*)
file(GLOB_RECURSE natpunching_sources src/natpunching/*)
file(GLOB_RECURSE net_sources src/net/*)
file(GLOB_RECURSE parser_sources src/parser/*)
file(GLOB_RECURSE dht_sources src/dht/*)
file(GLOB_RECURSE crypto_sources src/crypto/*)

# Contrib files
file(GLOB_RECURSE webm_sources contrib/webm/*)

# Library
add_library(epyx STATIC src/api.cpp
    ${core_sources}
    ${crypto_sources}
    ${dht_sources}
    ${n2np_sources}
    ${natpunching_sources}
    ${net_sources}
    ${parser_sources}
    ${tinyxml_sources}
)
target_link_libraries(epyx ${Boost_LIBRARIES} pthread cryptopp)

# Test executables
add_executable(test-thread src/test-thread.cpp)
target_link_libraries(test-thread epyx)

add_executable(test-server src/test-server.cpp)
target_link_libraries(test-server epyx)

add_executable(test-parser src/test-parser.cpp)
target_link_libraries(test-parser epyx)

add_executable(test-natpunching src/test-natpunching.cpp)
target_link_libraries(test-natpunching epyx)

add_executable(test-n2np src/test-n2np.cpp)
target_link_libraries(test-n2np epyx)

add_executable(demoframe src/demoframe.cpp)
target_link_libraries(demoframe epyx)

add_executable(test-dht src/test-dht.cpp)
target_link_libraries(test-dht epyx)

add_executable(test-crypto ${core_sources} ${crypto_sources} src/test-crypto.cpp)
target_link_libraries(test-crypto epyx)

add_executable(demo src/demo.cpp)
target_link_libraries(demo epyx)


add_executable(test-webm contrib/test-webm.cpp ${webm_sources})
target_link_libraries(test-webm epyx vpx SDL)

add_executable(demo-video ${webm_sources} contrib/demo-video.cpp)
target_link_libraries(demo-video epyx vpx SDL)

# Real programs
add_executable(simple-relay src/simple-relay.cpp)
target_link_libraries(simple-relay epyx)

add_executable(bootstrap src/bootstrap.cpp)
target_link_libraries(bootstrap epyx)
