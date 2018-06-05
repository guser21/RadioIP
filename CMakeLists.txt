cmake_minimum_required(VERSION 3.10)
project(Radio)

set(CMAKE_CXX_STANDARD 17)

find_package(GTest REQUIRED)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})


FIND_PACKAGE(Boost)
IF (Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
    ADD_DEFINITIONS("-DHAS_BOOST")
ENDIF ()

find_package(Threads)
SET(GCC_COVERAGE_COMPILE_FLAGS "-g")
SET(GCC_COVERAGE_LINK_FLAGS    "-lgcov")


find_package(Boost COMPONENTS program_options REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include ${Boost_INCLUDE_DIR})

link_libraries(${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

file(GLOB common_src
        "src/common/*.h"
        "src/common/*.cpp")


file(GLOB server_src
        "src/server/*.h"
        "src/server/*.cpp")

file(GLOB client_src
        "src/client/*.h"
        "src/client/*.cpp")

file(GLOB server_main src/server/server_main.cpp)
file(GLOB client_main src/client/client_main.cpp)

list(REMOVE_ITEM client_src ${client_main})
list(REMOVE_ITEM server_src ${server_main})

add_executable(Server ${server_main} ${server_src} ${common_src})
add_executable(Client ${client_main} ${client_src} ${common_src})


add_subdirectory(test)
enable_testing()
