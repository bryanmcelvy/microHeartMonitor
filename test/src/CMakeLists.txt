#**************************************************************************************************
# File:           /test/src/CMakeLists.txt
# Description:    TODO
#**************************************************************************************************
include_directories(${CPPUTEST_INCLUDE_DIR})
# link_libraries(${CPPUTEST_LIB_TARG} ${CPPUTEST_EXT_LIB_TARG})

# Main Test Runner
add_executable(testRunner_All testRunner_All.cpp)
target_include_directories(testRunner_All PRIVATE ${CPPUTEST_INCLUDE_DIR})
target_link_libraries(testRunner_All ${CPPUTEST_LIB_TARG} ${CPPUTEST_EXT_LIB_TARG})

# Led Tests
add_library(testGroup_Led OBJECT testGroup_Led.cpp ${PATH_MIDDLEWARE}/Led.c)
target_include_directories(testGroup_Led PUBLIC ${PATH_MIDDLEWARE} ${PATH_COMMON} ${PATH_DRIVERS} ${PATH_DEVICE})
target_compile_definitions(testGroup_Led PUBLIC LED_POOL_SIZE=15)
target_link_libraries(testRunner_All testGroup_Led stub_GPIO stub_NewAssert)

# FIFO Tests
add_library(testGroup_FIFO OBJECT testGroup_FIFO.cpp ${PATH_COMMON}/FIFO.c)
target_include_directories(testGroup_FIFO PUBLIC ${PATH_COMMON})
target_compile_definitions(testGroup_FIFO PUBLIC FIFO_POOL_SIZE=25)
target_link_libraries(testRunner_All testGroup_FIFO)
