#[[*********************************************************************************************
File:           CMakeLists.txt
Description:    Sets minimum CMake version, links toolchain file, declares
                project, and adds subdirectories
#***********************************************************************************************    ]]
cmake_minimum_required(VERSION 3.20)
set(CMAKE_TOOLCHAIN_FILE "tm4c123g.cmake")          # must be placed before project()
project(ECG_HRM C)

# Build Options
option(OPT_GEN_DOC      "Generate documentation using Doxygen and pdflatex"         OFF)
option(OPT_CPPCHECK     "Run static code analysis (i.e. linting) via CppCheck"      ON)
option(OPT_TESTING      "Build test scripts for various modules"                    ON)

# CppCheck Configuration
if(OPT_CPPCHECK)
    find_program(CMAKE_C_CPPCHECK NAMES cppcheck)
    if(CMAKE_C_CPPCHECK)
        list(APPEND CMAKE_C_CPPCHECK "\
                --enable=all;\
                --error-exitcode=1;\
                --suppress=unusedFunction;\
                --verbose;\
                --cppcheck-build-dir=${PROJECT_BINARY_DIR}/cppcheck;\
                --clang;\
                -i ${PROJECT_SOURCE_DIR}/src/device\
                ")
    endif()
    
endif()

# Subdirectories
add_subdirectory(src)                               # source code, libraries, test scripts
add_subdirectory(docs)                              # datasheets and generated code documentation
