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

add_executable(Server
        src/control_daemon.cpp
        src/err.cpp
        src/server_main.cpp
        src/streaming_service.cpp)
add_executable(Client
        src/err.cpp
        src/reciever_service.cpp
        src/client_main.cpp)

