#[[*********************************************************************************************
File:           src/common/CMakeLists.txt
Description:    Adds static libraries used by both application software and device drivers.
#***********************************************************************************************    ]]

add_library(FIFO STATIC FIFO.c FIFO.h)

add_library(Filter STATIC Filter.c Filter.h)
target_link_libraries(Filter PRIVATE FIFO)
