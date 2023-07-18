#[[*********************************************************************************************
File:           CMakeLists.txt
Description:    Sets minimum CMake version, links toolchain file, declares
                project, and adds subdirectories
#***********************************************************************************************    ]]
cmake_minimum_required(VERSION 3.20)
set(CMAKE_TOOLCHAIN_FILE "tm4c123g.cmake")          # must be placed before project()
project(ECG_HRM C)

option(OPT_GEN_DOC "Generate documentation using Doxygen and pdflatex" OFF)
option(OPT_TESTING "Build test scripts for various modules" OFF)

add_subdirectory(src)                               # source code, libraries, test scripts
add_subdirectory(docs)                              # datasheets and generated code documentation
