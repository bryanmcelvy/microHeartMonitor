#**************************************************************************************************
# File:         /docs/CMakeLists.txt
# Description:  Generate documentation using Doxygen and pdfLaTeX.
#**************************************************************************************************
find_package(Doxygen REQUIRED dot)
find_package(LATEX COMPONENTS PDFLATEX)
find_package(Python REQUIRED)

if(DOXYGEN_FOUND AND LATEX_FOUND)
    set(LATEX_OUTPUT_DIR ${PROJECT_BINARY_DIR}/docs/latex)

    add_custom_target(  gen_docs
                        VERBATIM
                        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                        COMMAND python docs/doc_gen.py
    )

    set_target_properties(gen_docs PROPERTIES EXCLUDE_FROM_ALL TRUE)
endif()
