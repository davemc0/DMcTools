# DMcToolsTest Solution

cmake_minimum_required(VERSION 3.13)

set(EXE_NAME DMcToolsTest)

project(${EXE_NAME})

set(CMAKE_CXX_STANDARD 11 CACHE STRING "" FORCE)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(PROJECT_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# warning level, all warnings as errors, build in parallel, disable security warnings, latest language support
add_compile_options(/W3 /WX /MP /sdl- /std:c++latest)

set(__ARCH "x64")

set(SOURCES
    DMcToolsTest.cpp
    DiskReadSpeedTest.cpp
    GaussianTest.cpp
    HashStringTest.cpp
    ImageRWSpeedTest.cpp
    KDTreeTest.cpp
    MappingsTest.cpp
    Matrix44Test.cpp
    PullPushTest.cpp
    TimerTest.cpp
    VCDTest.cpp
    tImageTest.cpp
)

source_group("src"  FILES ${SOURCES})

add_subdirectory(${PROJECT_ROOT_DIR}/../../../DMcTools ${CMAKE_CURRENT_BINARY_DIR}/DMcTools)

add_executable(${EXE_NAME} ${SOURCES})

set_target_properties(${EXE_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_ROOT_DIR} )
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${EXE_NAME})

target_link_libraries(${EXE_NAME} PRIVATE DMcTools)
