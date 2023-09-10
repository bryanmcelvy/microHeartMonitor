#[[*********************************************************************************************
File:           /test/mocks/CMakeLists.txt
Description:    TODO
#***********************************************************************************************    ]]
include_directories(${CPPUTEST_INCLUDE_DIR})
link_libraries(${CPPUTEST_LIB_TARGS})

add_library(mock_GPIO mock_GPIO.cpp)
target_include_directories(mock_GPIO PUBLIC ${PATH_COMMON} ${PATH_DEVICE} ${PATH_DRIVERS})
