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
option(OPT_CPPCHECK     "Run static code analysis (i.e. linting) via cppcheck when building `all`"      ON)
option(OPT_TESTING      "Build test scripts when building `all`"                                        ON)

# Useful Variables
set(PATH_DOCS ${PROJECT_SOURCE_DIR}/docs)

set(PATH_TOOLS ${PROJECT_SOURCE_DIR}/tools)
set(PATH_CPPCHECK ${PATH_TOOLS}/cppcheck)

set(PATH_SRC ${PROJECT_SOURCE_DIR}/src)

set(PATH_EXTERNAL ${PATH_SRC}/external)
list(APPEND CMSIS_DIRS                  
                ${PATH_EXTERNAL}/CMSIS-DSP/Include
                ${PATH_EXTERNAL}/CMSIS-DSP/PrivateInclude
                ${PATH_EXTERNAL}/CMSIS-DSP/Source
                ${PATH_EXTERNAL}/CMSIS/Core/Include)

# Subdirectories
add_subdirectory(${PATH_SRC})                               # source code, libraries, test scripts
add_subdirectory(${PATH_DOCS})                              # datasheets and generated code documentation
add_subdirectory(${PROJECT_SOURCE_DIR}/test)

# Cppcheck Configuration
find_program(CMAKE_C_CPPCHECK NAMES cppcheck)
if(CMAKE_C_CPPCHECK)
    list(APPEND CPPCHECK_FLAGS
                    "--enable=all"
                    "--error-exitcode=1"
                    "--quiet"
                    "--suppressions-list=${PATH_CPPCHECK}/suppressions.txt"
                    "--template=gcc"
                    "--relative-paths=${PROJECT_SOURCE_DIR}"
                    "--cppcheck-build-dir=${PATH_CPPCHECK}"
                    "--project=${PROJECT_BINARY_DIR}/compile_commands.json"
                    "-i${PATH_EXTERNAL}")

    add_custom_target(lint 
                        COMMENT             "Running cppcheck:"
                        WORKING_DIRECTORY   ${PROJECT_SOURCE_DIR}
                        COMMAND             ${CMAKE_C_CPPCHECK} ${CPPCHECK_FLAGS} ${PATH_SRC}
                        VERBATIM)

    if(OPT_CPPCHECK)
        set_target_properties(lint PROPERTIES EXCLUDE_FROM_ALL FALSE)
    else()
        set_target_properties(lint PROPERTIES EXCLUDE_FROM_ALL TRUE)
    endif()
endif()
