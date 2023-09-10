#[[***********************************************************************************************
File:           /src/CMakeLists.txt
Description:    Adds subdirectories and main target executable
**************************************************************************************************  ]]

# Path Variables
set(PATH_APP ${PATH_SRC}/app)
set(PATH_COMMON ${PATH_SRC}/common)
set(PATH_DEVICE ${PATH_SRC}/device)
set(PATH_DRIVERS ${PATH_SRC}/drivers)
set(PATH_MIDDLEWARE ${PATH_SRC}/middleware)
set(PATH_TEST ${PATH_SRC}/test)

# Add startup file as Object Library
add_library(Startup OBJECT ${PATH_DEVICE}/startup_gcc.c)

# Subdirectories
add_subdirectory(${PATH_APP})
add_subdirectory(${PATH_COMMON})
add_subdirectory(${PATH_DRIVERS})
add_subdirectory(${PATH_MIDDLEWARE})
add_subdirectory(${PATH_TEST})

# Main Target Declaration
set(MAIN_TARGET main.elf)

add_executable(${MAIN_TARGET} main.c)
target_include_directories( ${MAIN_TARGET} 
                            PRIVATE 
                                ${PATH_APP}
                                ${PATH_COMMON}
                                ${PATH_DEVICE}
                                ${PATH_DRIVERS}
                                ${PATH_MIDDLEWARE}
                                ${CMSIS_DIRS}
                            )
target_link_libraries(  ${MAIN_TARGET} 
                        PRIVATE 
                            Startup
                            DAQ   Debug  LCD   QRS   UserCtrl       # app libraries
                            PLL                             # driver libraries
                        )
