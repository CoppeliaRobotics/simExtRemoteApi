cmake_minimum_required(VERSION 3.15)
project(simExtRemoteApi)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_MACOSX_RPATH 1)

find_package(Boost REQUIRED)
if(UNIX AND NOT APPLE)
    find_package(Threads REQUIRED)
endif()

if(NOT LIBPLUGIN_DIR)
    if(DEFINED ENV{COPPELIASIM_ROOT_DIR})
        set(LIBPLUGIN_DIR $ENV{COPPELIASIM_ROOT_DIR}/programming/libPlugin)
    else()
        set(LIBPLUGIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../libPlugin)
    endif()
endif()
list(APPEND CMAKE_MODULE_PATH ${LIBPLUGIN_DIR}/cmake)
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
    ${COPPELIASIM_COMMON_DIR}/scriptFunctionData.cpp
    ${COPPELIASIM_COMMON_DIR}/scriptFunctionDataItem.cpp
    ${COPPELIASIM_COMMON_DIR}/shared_memory.c
)
if(UNIX AND NOT APPLE)
    target_link_libraries(simExtRemoteApi Threads::Threads rt)
endif()
