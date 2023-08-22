#[[*********************************************************************************************
File:         /docs/CMakeLists.txt
Description:  Generate documentation using Doxygen and pdfLaTeX.
#***********************************************************************************************    ]]
find_package(Doxygen REQUIRED dot)
find_package(LATEX COMPONENTS PDFLATEX)

if(DOXYGEN_FOUND AND LATEX_FOUND)
    set(LATEX_OUTPUT_DIR ${PROJECT_BINARY_DIR}/docs/latex)

    add_custom_target(  gen_docs
                        VERBATIM
                        WORKING_DIRECTORY ${PATH_DOCS}
                        COMMENT "Generating documentation..."
    
                        # remove the latex output directory
                        COMMAND ${CMAKE_COMMAND} -E rm -rf ${LATEX_OUTPUT_DIR}
                        
                        # generate call graph and pdf documentation
                        COMMAND dot -Tpng doxygen_files/call.dot -o call.png
                        COMMAND ${CMAKE_COMMAND} -E chdir ${PATH_DOCS}/doxygen_files doxygen
                        COMMAND ${CMAKE_COMMAND} -E chdir ${LATEX_OUTPUT_DIR} make -s

                        # copy new pdf and to `docs` directory
                        COMMAND ${CMAKE_COMMAND} -E copy ${LATEX_OUTPUT_DIR}/refman.pdf ${PATH_DOCS}
                        COMMAND ${CMAKE_COMMAND} -E copy ${LATEX_OUTPUT_DIR}/refman.log ${PATH_DOCS}/doxygen_files
                    )

    if (OPT_GEN_DOC)
        set_target_properties(gen_docs PROPERTIES EXCLUDE_FROM_ALL FALSE)
    endif()
endif()
