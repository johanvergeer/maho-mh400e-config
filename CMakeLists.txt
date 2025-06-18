cmake_minimum_required(VERSION 3.10)
project(LinuxCNCComponents C)

set(CMAKE_C_STANDARD 11)

set(LUBRICATION_COMP ./Components/src/Lubrication)
set(CALCULATE_COMP ./Components/src/Calculate)

include_directories(${LUBRICATION_COMP})
include_directories(${CALCULATE_COMP})

if (CMAKE_BUILD_TYPE STREQUAL "Test")
    set(UNITY_VENDOR ./Components/build/vendor/unity/src)
    set(CMOCK_VENDOR ./Components/build/vendor/cmock/src)
    include_directories(${UNITY_VENDOR})
    include_directories(${CMOCK_VENDOR})

    set(LUBRICATION_TESTS ./Components/test/Lubrication)
    include_directories(${LUBRICATION_TESTS})

    file(GLOB TEST_FILES "${LUBRICATION_TESTS}/*.c")
endif ()

set(
    SOURCE_FILES
    ${LUBRICATION_COMP}/lubrication_logic.c
    ${CALCULATE_COMP}/calculate_logic.c
)

if (CMAKE_BUILD_TYPE STREQUAL "Test")
    add_executable(dummy ${SOURCE_FILES} ${TEST_FILES})
else ()
    add_executable(dummy ${SOURCE_FILES})
endif ()
