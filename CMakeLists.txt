set(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE INTERNAL "" FORCE)
cmake_minimum_required(VERSION 3.15)
set(CMAKE_OSX_ARCHITECTURES x86_64)

project(simExtRemoteApi)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_MACOSX_RPATH 1)

set(CMAKE_THREAD_LIBS_INIT "-lpthread")
set(CMAKE_HAVE_THREADS_LIBRARY 1)
set(CMAKE_USE_WIN32_THREADS_INIT 0)
set(CMAKE_USE_PTHREADS_INIT 1)
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_OSX_SYSROOT "/Library/Developer/CommandLineTools/SDKs/MacOSX13.1.sdk")
set(Qt5_DIR "/opt/homebrew/Cellar/qt@5/5.15.8_2/lib/cmake/Qt5")
set(COPPELIASIM_ROOT_DIR "/Applications/coppeliaSim.app/Contents/Resources")

set( BOOST_ROOT /usr/local/homebrew/Cellar/boost/1.81.0_1 CACHE PATH "Boost library path" )
set( Boost_NO_SYSTEM_PATHS on CACHE BOOL "Do not search system for Boost" )
set( Boost_NO_BOOST_CMAKE on CACHE BOOL "Do not search system for Boost" )

set( Boost_INCLUDE_DIRS ${BOOST_ROOT}/include )

include_directories(${Boost_INCLUDE_DIRS})

link_directories(/Applications/coppeliaSim.app/Contents/MacOS)

find_package(Boost REQUIRED)
find_package(Boost REQUIRED COMPONENTS regex)

if(UNIX AND NOT APPLE)
    find_package(Threads REQUIRED)
endif()

if(NOT COPPELIASIM_INCLUDE_DIR)
    if(DEFINED COPPELIASIM_ROOT_DIR)
        set(COPPELIASIM_INCLUDE_DIR ${COPPELIASIM_ROOT_DIR}/programming/include)
    else()
        set(COPPELIASIM_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include)
    endif()
endif()
list(APPEND CMAKE_MODULE_PATH ${COPPELIASIM_INCLUDE_DIR}/cmake)
message(${COPPELIASIM_INCLUDE_DIR})
find_package(CoppeliaSim   REQUIRED)


include_directories(${COPPELIA_INCLUDE_DIR})
include_directories(${COPPELIASIM_INCLUDE_DIR})
include_directories(${COPPELIASIM_INCLUDE_DIR}/simLib)

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
file(GLOB LIBRARIES "/Applications/coppeliaSim.app/Contents/MacOS/*.dylib")
message("LIBRARIES = ${LIBRARIES}")
target_link_libraries(simExtRemoteApi ${LIBRARIES})