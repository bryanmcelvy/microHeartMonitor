#[[*********************************************************************************************
File:           /src/common/CMakeLists.txt
Description:    Adds object libraries used by both application software and device drivers.
#***********************************************************************************************    ]]
add_library(FIFO OBJECT FIFO.c FIFO.h)

add_library(ISR OBJECT ISR.c ISR.h)
target_include_directories(ISR PUBLIC ${PATH_DEVICE})

add_library(Lookup OBJECT lookup.c lookup.h)
target_include_directories(Lookup PRIVATE ${CMSIS_DIRS})
target_link_libraries(Lookup CMSIS-DSP-f32)

add_library(NewAssert OBJECT NewAssert.c NewAssert.h)
