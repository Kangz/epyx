# Run cmake . to build Makefile

cmake_minimum_required(VERSION 2.6)

# Project name
project(Epyx)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

SET(CMAKE_CXX_FLAGS "-g -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable")

# Make some sets of files from directories
file(GLOB_RECURSE tinyxml_sources include/tinyxml/*)
file(GLOB_RECURSE core_sources src/core/*)
file(GLOB_RECURSE localn2np_sources src/localn2np/*)
file(GLOB_RECURSE n2np_sources src/n2np/*)
file(GLOB_RECURSE natpunching_sources src/natpunching/*)
file(GLOB_RECURSE net_sources src/net/*)
file(GLOB_RECURSE parser_sources src/parser/*)
file(GLOB_RECURSE dht_sources src/dht/*)
file(GLOB_RECURSE crypto_sources src/crypto/*)

# More general file sets, with deps
file(GLOB epyx_api_sources src/api.cpp
    ${core_sources} ${n2np_sources} ${net_sources} ${parser_sources})

# Test executables
add_executable(test-thread ${epyx_api_sources} src/test-thread.cpp)
target_link_libraries(test-thread pthread)

add_executable(test-server ${epyx_api_sources} src/test-server.cpp)
target_link_libraries(test-server pthread)

add_executable(test-parser ${epyx_api_sources} src/test-parser.cpp)
target_link_libraries(test-parser pthread)

add_executable(test-natpunching ${epyx_api_sources} src/test-natpunching.cpp
    ${natpunching_sources} ${tinyxml_sources})
target_link_libraries(test-natpunching pthread)

add_executable(test-local-n2np ${epyx_api_sources} src/test-local-n2np.cpp
    ${localn2np_sources})
target_link_libraries(test-local-n2np pthread)

add_executable(test-n2np ${epyx_api_sources} src/test-n2np.cpp)
target_link_libraries(test-n2np pthread)

add_executable(test-dht ${epyx_api_sources} src/test-dht.cpp
    ${dht_sources} ${localn2np_sources})
target_link_libraries(test-dht pthread)

add_executable(test-crypto ${core_sources} ${crypto_sources} src/test-crypto.cpp)
target_link_libraries(test-crypto pthread cryptopp)

# Real programs
add_executable(simple-relay ${epyx_api_sources} src/simple-relay.cpp)
target_link_libraries(simple-relay pthread)
