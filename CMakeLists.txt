#[[*********************************************************************************************
File:           /CMakeLists.txt
Description:    Sets minimum CMake version, links toolchain file, and declares project;
                adds subdirectories, options, and path variables; and configures Cppcheck.
#***********************************************************************************************    ]]
cmake_minimum_required(VERSION 3.20)
set(CMAKE_TOOLCHAIN_FILE "tm4c123g.cmake")          # must be placed before project()
project(ecg_hrm C)

# Build Options
option(OPT_GEN_DOC      "Generate Doxygen documentation when building `all`"                            OFF)
option(OPT_CPPCHECK     "Run static code analysis (i.e. linting) via cppcheck when building `all`"      ON)
option(OPT_TESTING      "Build test scripts when building `all`"                                        ON)

# Useful Variables
set(PATH_SRC ${PROJECT_SOURCE_DIR}/src)

set(PATH_APP ${PATH_SRC}/app)
set(PATH_COMMON ${PATH_SRC}/common)
set(PATH_DEVICE ${PATH_SRC}/device)
set(PATH_DRIVERS ${PATH_SRC}/drivers)
set(PATH_EXTERNAL ${PATH_SRC}/external)
set(PATH_TEST ${PATH_SRC}/test)

list(APPEND 
        CMSIS_DIRS                  
            ${PATH_EXTERNAL}/CMSIS-DSP/Include
            ${PATH_EXTERNAL}/CMSIS-DSP/PrivateInclude
            ${PATH_EXTERNAL}/CMSIS-DSP/Source
            ${PATH_EXTERNAL}/CMSIS/Core/Include)

# Subdirectories
add_subdirectory(src)                               # source code, libraries, test scripts
add_subdirectory(docs)                              # datasheets and generated code documentation

# Cppcheck Configuration
find_program(CMAKE_C_CPPCHECK NAMES cppcheck)
if(CMAKE_C_CPPCHECK)
    set(CPPCHECK_DIR ${PROJECT_SOURCE_DIR}/cppcheck)
    set(CPPCHECK_FLAGS          "--enable=all" "--error-exitcode=1" "--quiet" "--force")
    list(APPEND CPPCHECK_FLAGS  "--suppressions-list=${CPPCHECK_DIR}/suppressions.txt" "--template=gcc")
    list(APPEND CPPCHECK_FLAGS  "--relative-paths=${PROJECT_SOURCE_DIR}" "--cppcheck-build-dir=${CPPCHECK_DIR}" "--project=${PROJECT_BINARY_DIR}/compile_commands.json")

    add_custom_target(lint 
                        COMMENT             "Running cppcheck:"
                        WORKING_DIRECTORY   ${PROJECT_SOURCE_DIR}
                        COMMAND             ${CMAKE_C_CPPCHECK} ${CPPCHECK_FLAGS} -i${PATH_EXTERNAL} ${PATH_SRC}
                        VERBATIM)

    if(OPT_CPPCHECK)
        set_target_properties(lint PROPERTIES EXCLUDE_FROM_ALL FALSE)
    else()
        set_target_properties(lint PROPERTIES EXCLUDE_FROM_ALL TRUE)
    endif()
endif()
