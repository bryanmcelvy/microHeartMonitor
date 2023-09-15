#[[*********************************************************************************************
File:           /test/CMakeLists.txt
Description:    Subproject for on-host unit testing.
#***********************************************************************************************    ]]
project(ecg_hrm_unit_tests C CXX)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic -std=c99")

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -std=c++11")

set(CMAKE_EXE_LINKER_FLAGS "")

#*****************************************************************************
# Subdirectories
#*****************************************************************************
# add_subdirectory(fakes)
add_subdirectory(mocks)
add_subdirectory(stubs)

add_subdirectory(src)

#*****************************************************************************
# Custom Targets
#*****************************************************************************
add_custom_target(run_tests
                    ALL 
                    DEPENDS testRunner_All
                    VERBATIM
                    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/src
                    COMMENT "\n\n***************\nRunning unit tests...\n***************\n\n"
                    COMMAND ./testRunner_All -v)

# find_program(GCOVR NAMES gcovr)
# if(GCOVR)
#     message(STATUS "gcovr found.")
#     cmake_path(GET PROJECT_SOURCE_DIR PARENT_PATH TRUE_SRC_PATH)

#     add_custom_target(generate_coverage_report
#                         VERBATIM
#                         WORKING_DIRECTORY ${TRUE_SRC_PATH}
#                         COMMENT "\n\n***************\nGenerating coverage report...\n***************\n\n"
#                         COMMAND gcovr -o coverage.lcov
#     )
#     add_dependencies(run_tests generate_coverage_report)
# endif()
