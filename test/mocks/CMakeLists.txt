#**************************************************************************************************
# File:           /test/mocks/CMakeLists.txt
# Description:    TODO
#**************************************************************************************************
include_directories(${CPPUTEST_INCLUDE_DIR})
link_libraries(${CPPUTEST_LIB_TARG} ${CPPUTEST_EXT_LIB_TARG})

add_library(mock_GPIO OBJECT mock_GPIO.cpp)
target_include_directories(mock_GPIO PUBLIC ${PATH_COMMON} ${PATH_DEVICE} ${PATH_DRIVERS})
