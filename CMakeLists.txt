cmake_minimum_required(VERSION 3.15)
project(simExtRemoteApi)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_MACOSX_RPATH 1)

find_package(Boost REQUIRED)
if(UNIX AND NOT APPLE)
    find_package(Threads REQUIRED)
endif()

if(NOT COPPELIASIM_INCLUDE_DIR)
    if(DEFINED ENV{COPPELIASIM_ROOT_DIR})
        set(COPPELIASIM_INCLUDE_DIR $ENV{COPPELIASIM_ROOT_DIR}/programming/include)
    else()
        set(COPPELIASIM_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include)
    endif()
endif()
list(APPEND CMAKE_MODULE_PATH ${COPPELIASIM_INCLUDE_DIR}/cmake)
find_package(CoppeliaSim 4.1.0.0 REQUIRED)

coppeliasim_add_plugin(
    simExtRemoteApi
    LEGACY
    SOURCES
    simExtRemoteApi.cpp
    confReader.cpp
    inConnection.cpp
    porting.cpp
    simxCmd.cpp
    simxConnections.cpp
    simxContainer.cpp
    simxSocket.cpp
    simxUtils.cpp
    ${COPPELIASIM_INCLUDE_DIR}/simLib/scriptFunctionData.cpp
    ${COPPELIASIM_INCLUDE_DIR}/simLib/scriptFunctionDataItem.cpp
    ${COPPELIASIM_INCLUDE_DIR}/simLib/shared_memory.c
)
if(UNIX AND NOT APPLE)
    target_link_libraries(simExtRemoteApi Threads::Threads rt)
endif()
