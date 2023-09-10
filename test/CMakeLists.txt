#[[*********************************************************************************************
File:           /test/CMakeLists.txt
Description:    TODO
#***********************************************************************************************    ]]
project(ecg_hrm_unit_tests C CXX)
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_C_FLAGS "-ffunction-sections -fdata-sections -Wall -Wextra -pedantic -nostartfiles")

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "-ffunction-sections -fdata-sections -Wall -Wextra -pedantic -nostartfiles")

include(cpputest.cmake)
add_subdirectory(src)
add_subdirectory(mocks)
