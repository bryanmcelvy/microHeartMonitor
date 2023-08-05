#[[*********************************************************************************************
File:         docs/CMakeLists.txt
Description:  generate documentation using Doxygen and pdfLaTeX
#***********************************************************************************************    ]]
find_package(Doxygen 
                REQUIRED 
                    dot
                )
find_package(LATEX 
                COMPONENTS 
                    PDFLATEX
                )

if( (DOXYGEN_FOUND AND LATEX_FOUND) )
    
    set(LATEX_OUTPUT_DIR ${PROJECT_BINARY_DIR}/docs/latex)
    add_custom_target(  gen_docs
                        COMMENT     "Generating documentation..."
    
                        # remove the latex output directory and the previous log
                        COMMAND ${CMAKE_COMMAND} -E rm -r -f ${LATEX_OUTPUT_DIR} ${PROJECT_SOURCE_DIR}/docs/doxygen/refman.log
                        
                        # run Doxygen using the config file `doxyfile` and run `make` to gen. pdf and log
                        COMMAND ${CMAKE_COMMAND} -E chdir ${PROJECT_SOURCE_DIR}/docs/doxygen doxygen
                        COMMAND ${CMAKE_COMMAND} -E chdir ${LATEX_OUTPUT_DIR} make -s

                        # copy new pdf and log to `docs` directory
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${LATEX_OUTPUT_DIR}/refman.pdf ${PROJECT_SOURCE_DIR}/docs
                        COMMAND ${CMAKE_COMMAND} -E copy ${LATEX_OUTPUT_DIR}/refman.log ${PROJECT_SOURCE_DIR}/docs/doxygen
                        
                        VERBATIM
                        )
    if (OPT_GEN_DOC)
        set_target_properties(  gen_docs
                                    PROPERTIES
                                        EXCLUDE_FROM_ALL    FALSE)
    endif()

endif()
