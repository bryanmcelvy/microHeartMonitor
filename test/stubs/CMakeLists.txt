#**************************************************************************************************
# File:           /test/stubs/CMakeLists.txt
# Description:    TODO
#**************************************************************************************************
include_directories(${CPPUTEST_INCLUDE_DIR})
link_libraries(${CPPUTEST_LIB_TARG} ${CPPUTEST_EXT_LIB_TARG})

add_library(stub_NewAssert OBJECT stub_NewAssert.cpp)
target_include_directories(stub_NewAssert PUBLIC ${PATH_COMMON})

add_library(stub_GPIO OBJECT stub_GPIO.c)
target_include_directories(stub_GPIO PUBLIC ${PATH_COMMON} ${PATH_DEVICE} ${PATH_DRIVERS})

