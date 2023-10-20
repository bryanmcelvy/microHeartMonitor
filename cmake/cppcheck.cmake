#**************************************************************************************************
# File:           /cmake/cppcheck.cmake
# Description:    Sets Cppcheck configuration.
#**************************************************************************************************
find_program(CPPCHECK_EXEC NAMES cppcheck)
if(CPPCHECK_EXEC)
    list(APPEND CMAKE_C_CPPCHECK
                    "--enable=all"
                    "--error-exitcode=1"
                    "--quiet"
                    "--suppressions-list=${PATH_CPPCHECK}/suppressions.txt"
                    "--template=gcc"
                    "--relative-paths=${PROJECT_SOURCE_DIR}"
                    "--cppcheck-build-dir=${PATH_CPPCHECK}"
                    "--project=${PROJECT_BINARY_DIR}/compile_commands.json"
                    "-i${PATH_EXTERNAL}")
endif()
