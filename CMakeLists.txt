cmake_minimum_required(VERSION 3.10)
project(LinuxCNCComponents C)

set(CMAKE_CXX_STANDARD 11)

set(LUBRICATION_COMP ./Components/src/Lubrication)
set(CALCULATE_COMP ./Components/src/Calculate)

# Files created by Ceedling for testing
set(UNITY_VENDOR ./Components/build/vendor/unity/src)

# Voeg de include directories toe zodat CLion de headers kan vinden
include_directories(${LUBRICATION_COMP})
include_directories(${CALCULATE_COMP})
include_directories(${UNITY_VENDOR})

# Verzamel alle C-bestanden (dit is puur zodat CLion weet wat er is)
set(
        SOURCE_FILES
        ${LUBRICATION_COMP}/lubrication_logic.c
        ${CALCULATE_COMP}/calculate_logic.c
)

# Maak een dummy executable (je hoeft dit niet echt te bouwen)
#add_executable(dummy ${SOURCE_FILES})
