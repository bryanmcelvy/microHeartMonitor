#[[*********************************************************************************************
File:           CMakeLists.txt
Description:    Sets minimum CMake version, links toolchain file, declares
                project, and adds subdirectories
#***********************************************************************************************    ]]
cmake_minimum_required(VERSION 3.20)
set(CMAKE_TOOLCHAIN_FILE "tm4c123g.cmake")          # must be placed before project()
project(ecg_hrm C)

# Build Options
option(OPT_GEN_DOC      "Generate Doxygen documentation when building `all`"                            OFF)
option(OPT_CPPCHECK     "Run static code analysis (i.e. linting) via cppcheck when building `all`"      ON)
option(OPT_TESTING      "Build test scripts when building `all`"                                        ON)

# Subdirectories
add_subdirectory(src)                               # source code, libraries, test scripts
add_subdirectory(docs)                              # datasheets and generated code documentation

# cppcheck Configuration
find_program(CMAKE_C_CPPCHECK NAMES cppcheck)
if(CMAKE_C_CPPCHECK)
    set(CPPCHECK_FLAGS "--enable=all" "--error-exitcode=1" "--quiet")
    list(APPEND CPPCHECK_FLAGS "--suppress=unusedFunction" "--suppress=missingIncludeSystem" "--suppress=*:src/device/startup_gcc.c")
    list(APPEND CPPCHECK_FLAGS "--cppcheck-build-dir=${PROJECT_BINARY_DIR}/cppcheck" "--project=${PROJECT_BINARY_DIR}/compile_commands.json" "src")

    add_custom_target(lint 
                        COMMENT             "Running cppcheck:"
                        WORKING_DIRECTORY   ${PROJECT_SOURCE_DIR}/src
                        COMMAND             ${CMAKE_C_CPPCHECK} ${CPPCHECK_FLAGS}
                        VERBATIM)

    if(OPT_CPPCHECK)
        set_target_properties(lint PROPERTIES EXCLUDE_FROM_ALL FALSE)
    else()
        set_target_properties(lint PROPERTIES EXCLUDE_FROM_ALL FALSE)
    endif()
endif()
