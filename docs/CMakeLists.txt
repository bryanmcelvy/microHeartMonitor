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
                        
                        # generate images
                        COMMAND dot -Tpng ${PATH_DOXYGEN}/call.dot -o call.png
                        COMMAND dot -Tpng ${PATH_DOXYGEN}/data_flow.dot -o data_flow.png
                        COMMAND dot -Tpng ${PATH_DOXYGEN}/main1_init.dot -o main1_init.png
                        COMMAND dot -Tpng ${PATH_DOXYGEN}/main2_superloop.dot -o main2_superloop.png
                        COMMAND dot -Tpng ${PATH_DOXYGEN}/main3_daq.dot -o main3_daq.png
                        COMMAND dot -Tpng ${PATH_DOXYGEN}/main4_proc.dot -o main4_proc.png
                        COMMAND dot -Tpng ${PATH_DOXYGEN}/qrs_preproc.dot -o qrs_preproc.png

                        # generate pdf documentation
                        COMMAND ${CMAKE_COMMAND} -E chdir ${PATH_DOXYGEN} doxygen
                        COMMAND ${CMAKE_COMMAND} -E chdir ${LATEX_OUTPUT_DIR} make -s

                        # copy new pdf and reflog to `docs` directory
                        COMMAND ${CMAKE_COMMAND} -E copy ${LATEX_OUTPUT_DIR}/refman.pdf ${PATH_DOCS}
                        COMMAND ${CMAKE_COMMAND} -E copy ${LATEX_OUTPUT_DIR}/refman.log ${PATH_DOXYGEN}
    )

    set_target_properties(gen_docs PROPERTIES EXCLUDE_FROM_ALL TRUE)
endif()
