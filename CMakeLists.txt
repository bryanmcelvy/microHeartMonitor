#[[*********************************************************************************************
File:           /CMakeLists.txt
Description:    Sets minimum CMake version, links toolchain file, and declares project;
                adds subdirectories, options, and path variables; and configures Cppcheck.
#***********************************************************************************************    ]]
cmake_minimum_required(VERSION 3.20)
set(CMAKE_TOOLCHAIN_FILE "src/device/tm4c123g.cmake")          # must be placed before project()
project(ecg_hrm C)

# Build Options
option(OPT_GEN_DOC      "Generate Doxygen documentation when building `all`"                            OFF)
option(OPT_CPPCHECK     "Run static code analysis (i.e. linting) via cppcheck when building `all`"      OFF)
option(OPT_TESTING      "Build test scripts when building `all`"                                        ON)

# Useful Variables
set(PATH_DOCS ${PROJECT_SOURCE_DIR}/docs)
set(PATH_EXTERNAL ${PROJECT_SOURCE_DIR}/external)
set(PATH_SRC ${PROJECT_SOURCE_DIR}/src)
set(PATH_TOOLS ${PROJECT_SOURCE_DIR}/tools)
set(PATH_CPPCHECK ${PATH_TOOLS}/cppcheck)

set(PATH_APP ${PATH_SRC}/app)
set(PATH_COMMON ${PATH_SRC}/common)
set(PATH_DEVICE ${PATH_SRC}/device)
set(PATH_DRIVERS ${PATH_SRC}/drivers)
set(PATH_MIDDLEWARE ${PATH_SRC}/middleware)
set(PATH_TEST_SCRIPTS ${PATH_SRC}/test)

# Subdirectories
add_subdirectory(${PATH_DOCS})                              # datasheets and generated code documentation
add_subdirectory(${PATH_SRC})                               # source code, libraries, test scripts
add_subdirectory(${PROJECT_SOURCE_DIR}/test EXCLUDE_FROM_ALL)
add_subdirectory(${PATH_EXTERNAL})                          # ARM CMSIS-DSP library

# Cppcheck Configuration
find_program(CPPCHECK_EXEC NAMES cppcheck)
if(CPPCHECK_EXEC)
    list(APPEND CMAKE_C_CPPCHECK
                    "--enable=all"
                    "--error-exitcode=1"
                    "--quiet"
                    "--suppressions-list=${PATH_CPPCHECK}/suppressions.txt"
                    "--template=gcc"
                    "--relative-paths=${PROJECT_SOURCE_DIR}"
                    "--cppcheck-build-dir=${PATH_CPPCHECK}"
                    "--project=${PROJECT_BINARY_DIR}/compile_commands.json"
                    "-i${PATH_EXTERNAL}")
endif()
