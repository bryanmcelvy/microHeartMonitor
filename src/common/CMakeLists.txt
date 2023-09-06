#[[*********************************************************************************************
File:           /src/common/CMakeLists.txt
Description:    Adds object libraries used by both application software and device drivers.
#***********************************************************************************************    ]]
add_library(FIFO OBJECT FIFO.c FIFO.h)

add_library(Lookup OBJECT lookup.c lookup.h)
target_include_directories(Lookup PRIVATE ${CMSIS_DIRS})

add_library(NewAssert OBJECT NewAssert.c NewAssert.h)
