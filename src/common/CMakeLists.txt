#**************************************************************************************************
# File:           /src/common/CMakeLists.txt
# Description:    Adds object libraries used by both application software and device drivers.
#**************************************************************************************************
add_library(Fifo OBJECT Fifo.c Fifo.h)

add_library(NewAssert OBJECT NewAssert.c NewAssert.h)
