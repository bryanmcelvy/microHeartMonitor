#[[*********************************************************************************************
File:           /test/src/CMakeLists.txt
Description:    TODO
#***********************************************************************************************    ]]
include_directories(${CPPUTEST_INCLUDE_DIR})

add_library(test_Led OBJECT test_Led.cpp)
target_include_directories(test_Led PRIVATE
                            ${PATH_COMMON} ${PATH_DEVICE} ${PATH_DRIVERS} ${PATH_MIDDLEWARE}
                        )
