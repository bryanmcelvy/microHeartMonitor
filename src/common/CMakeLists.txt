#[[*********************************************************************************************
File:           src/common/CMakeLists.txt
Description:    Adds static libraries used by both application software and device drivers.
#***********************************************************************************************    ]]
add_library(FIFO STATIC FIFO.c FIFO.h)

add_library(Lookup INTERFACE lookup.c lookup.h)
target_sources(Lookup INTERFACE lookup.c lookup.h)
