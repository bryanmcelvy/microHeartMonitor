#**************************************************************************************************
# File:         /cmake/cpputest.cmake
# Description:  Adds pre-built CppUTest binaries as imported targets
#**************************************************************************************************
set(CPPUTEST_HOME $ENV{CPPUTEST})
set(CPPUTEST_INCLUDE_DIR ${CPPUTEST_HOME}/include)
set(CPPUTEST_LIB_DIR ${CPPUTEST_HOME}/lib)

add_library(CppUTest STATIC IMPORTED)
set_target_properties(CppUTest PROPERTIES IMPORTED_LOCATION "${CPPUTEST_LIB_DIR}/libCppUTest.a")

add_library(CppUTestExt STATIC IMPORTED)
set_target_properties(CppUTestExt PROPERTIES IMPORTED_LOCATION "${CPPUTEST_LIB_DIR}/libCppUTestExt.a")

set(CPPUTEST_LIB_TARG "CppUTest")
set(CPPUTEST_EXT_LIB_TARG "CppUTestExt")
