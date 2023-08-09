#[[*********************************************************************************************
File:           src/app/CMakeLists.txt
Description:    Adds application software libraries and link to driver libraries.
#***********************************************************************************************    ]]

include_directories(${CMAKE_SOURCE_DIR}/src/drivers
                    ${CMAKE_SOURCE_DIR}/src/common
                    ${CMAKE_SOURCE_DIR}/src/device)

add_library(Debug STATIC Debug.c Debug.h)
target_link_libraries(Debug PRIVATE UART)

add_library(Filter STATIC Filter.c Filter.h)
target_link_libraries(Filter PRIVATE FIFO)

add_library(LCD STATIC LCD.c LCD.h)
target_link_libraries(LCD PRIVATE ILI9341)

add_library(QRS STATIC QRS.c QRS.h)
target_link_libraries(QRS PRIVATE Filter)

add_library(UserCtrl STATIC UserCtrl.c UserCtrl.h)
target_link_libraries(UserCtrl 
                        PRIVATE 
                            GPIO 
                            Timer
                        )
