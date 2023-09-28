#[[*********************************************************************************************
File:           /CMakeLists.txt
Description:    Sets minimum CMake version, links toolchain file, and declares project;
                adds subdirectories, options, and path variables; and configures Cppcheck.
#***********************************************************************************************    ]]
cmake_minimum_required(VERSION 3.20)
set(CMAKE_TOOLCHAIN_FILE "cmake_files/tm4c123g.cmake")          # must be placed before project()
project(ecg_hrm C)

#*****************************************************************************
# Build Options
#*****************************************************************************
option(OPT_CPPCHECK     "Run static code analysis (i.e. linting) via cppcheck when building `all`"      OFF)
option(OPT_TESTING      "Build test scripts when building `all`"                                        ON)

#*****************************************************************************
# Path Variables
#*****************************************************************************
set(PATH_DOCS ${PROJECT_SOURCE_DIR}/docs)
set(PATH_EXTERNAL ${PROJECT_SOURCE_DIR}/external)
set(PATH_SRC ${PROJECT_SOURCE_DIR}/src)
set(PATH_UNIT_TESTS ${PROJECT_SOURCE_DIR}/test)

set(PATH_TOOLS ${PROJECT_SOURCE_DIR}/tools)
set(PATH_CPPCHECK ${PATH_TOOLS}/cppcheck)

set(PATH_APP ${PATH_SRC}/app)
set(PATH_COMMON ${PATH_SRC}/common)
set(PATH_DEVICE ${PATH_SRC}/device)
set(PATH_DRIVERS ${PATH_SRC}/drivers)
set(PATH_MIDDLEWARE ${PATH_SRC}/middleware)
set(PATH_TEST_SCRIPTS ${PATH_SRC}/test_scripts)

#*****************************************************************************
# Subdirectories
#*****************************************************************************
add_subdirectory(${PATH_DOCS})      # datasheets and generated code documentation
add_subdirectory(${PATH_EXTERNAL})  # ARM CMSIS libraries
add_subdirectory(${PATH_SRC})       # source code, libraries, test scripts

# Unit Testing
include(${PROJECT_SOURCE_DIR}/cmake_files/cpputest.cmake)
add_subdirectory(${PATH_UNIT_TESTS} EXCLUDE_FROM_ALL)

#*****************************************************************************
# Cppcheck Configuration
#*****************************************************************************
include(${PROJECT_SOURCE_DIR}/cmake_files/cppcheck.cmake)
