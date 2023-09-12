#[[*********************************************************************************************
File:           /test/src/CMakeLists.txt
Description:    TODO
#***********************************************************************************************    ]]
include_directories(${CPPUTEST_INCLUDE_DIR})
# link_libraries(${CPPUTEST_LIB_TARG} ${CPPUTEST_EXT_LIB_TARG})

# Main Test Runner
add_executable(testRunner_All testRunner_All.cpp)
target_include_directories(testRunner_All PRIVATE ${CPPUTEST_INCLUDE_DIR})
target_link_libraries(testRunner_All ${CPPUTEST_LIB_TARG} ${CPPUTEST_EXT_LIB_TARG})

# Led Tests
add_library(testGroup_Led OBJECT testGroup_Led.cpp ${PATH_MIDDLEWARE}/Led.c)
target_include_directories(testGroup_Led PUBLIC ${PATH_MIDDLEWARE} ${PATH_COMMON} ${PATH_DRIVERS} ${PATH_DEVICE})
target_link_libraries(testRunner_All testGroup_Led stub_GPIO stub_NewAssert)
