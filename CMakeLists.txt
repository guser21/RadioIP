cmake_minimum_required(VERSION 3.10)
project(Radio)

set(CMAKE_CXX_STANDARD 17)
FIND_PACKAGE(Boost)
IF (Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
    ADD_DEFINITIONS("-DHAS_BOOST")
ENDIF ()

find_package(Threads)


link_libraries(${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

include_directories(${CMAKE_SOURCE_DIR}/include)
file(GLOB common_src
        "src/common/*.h"
        "src/common/*.cpp")


file(GLOB server_src
        "src/server/*.h"
        "src/server/*.cpp")

file(GLOB client_src
        "src/client/*.h"
        "src/client/*.cpp")


add_executable(Server ${server_src} ${common_src})
add_executable(Client ${client_src} ${common_src} src/client/msg_parser.cpp include/client/msg_parser.h)

