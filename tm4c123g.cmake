#[[
Toolchain File for TM4C123GH6PM microcontroller.
]]#

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)   # stops CMake from testing the compiler
set(CMAKE_C_COMPILER_WORKS 1)

# Describe target platform for cross-compilation
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Set variables for toolchain components
set(TOOLCHAIN_PREFIX                            "arm-none-eabi")

set(CMAKE_C_COMPILER                            ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER                          ${TOOLCHAIN_PREFIX}-g++)
set(CMAKE_ASM_COMPILER                          ${TOOLCHAIN_PREFIX}-as)

set(CMAKE_LINKER                                ${TOOLCHAIN_PREFIX}-ld)
set(LINKER_SCRIPT                               ${PROJECT_SOURCE_DIR}/src/device/tm4c123gh6pm.ld)

set(CMAKE_AR                                    ${TOOLCHAIN_PREFIX}-ar)

set(CMAKE_OBJCOPY                               ${TOOLCHAIN_PREFIX}-objcopy)
set(CMAKE_OBJDUMP                               ${TOOLCHAIN_PREFIX}-objdump)

# compiler/assembler/linker flags
set(CPU_FLAGS                                   "-mcpu=cortex-m4 -mthumb")
set(FPU_FLAGS                                   "-mfloat-abi=hard -mfpu=fpv4-sp-d16")
set(COMMON_FLAGS                                "${CPU_FLAGS} -ffunction-sections -fdata-sections -Wall -Wdouble-promotion -pedantic")

set(CMAKE_ASM_FLAGS                             "${COMMON_FLAGS}")
set(CMAKE_C_FLAGS                               "${COMMON_FLAGS}")
set(CMAKE_CXX_FLAGS                             "${COMMON_FLAGS} -fno-exceptions -fno-rtti")

set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS           "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS         "")
set(CMAKE_EXE_LINKER_FLAGS                      "-Wl,--gc-sections -Wl,-Map=${PROJECT_NAME}.map,--cref -T${LINKER_SCRIPT}")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
