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
option(OPT_CPPCHECK     "Run static code analysis (i.e. linting) via CppCheck"      OFF)
option(OPT_CLANG_TIDY   "Run static code analysis (i.e. linting) via clang-tidy"    OFF)
option(OPT_TESTING      "Build test scripts for various modules"                    OFF)

# Subdirectories
add_subdirectory(src)                               # source code, libraries, test scripts
add_subdirectory(docs)                              # datasheets and generated code documentation

# CppCheck Configuration
if(OPT_CPPCHECK)
    find_program(CMAKE_C_CPPCHECK NAMES cppcheck)
    if(CMAKE_C_CPPCHECK)
        list(APPEND CMAKE_C_CPPCHECK "--enable=all;--cppcheck-build-dir=${PROJECT_BINARY_DIR};--clang")
    endif()
    
endif()

if(OPT_CLANG_TIDY)
    find_program(CMAKE_C_CLANG_TIDY NAMES clang-tidy)
endif()
