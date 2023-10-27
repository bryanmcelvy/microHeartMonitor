#**************************************************************************************************
# File:         /docs/doc_gen.py
# Description:  Generates images and documentation
#**************************************************************************************************
import os
from shutil import copy2, move, rmtree
from subprocess import run

'''NOTE: this script should be run from the root directory'''
PATH_ROOT = os.getcwd()

PATH_OUTPUT = f"{PATH_ROOT}/build/docs"
PATH_OUTPUT_HTML = f"{PATH_ROOT}/build/docs/html"
PATH_OUTPUT_LATEX = f"{PATH_ROOT}/build/docs/latex"

PATH_DOCS = f"{PATH_ROOT}/docs"
PATH_DOXYGEN = f"{PATH_DOCS}/doxygen"

print("Generating documentation...")

# generate images
os.chdir(PATH_DOCS)
for file in os.listdir(PATH_DOXYGEN):
    filename, file_extension = os.path.splitext(file)
    if file_extension == '.dot':
        run(['dot', '-Tpng', f"{PATH_DOXYGEN}/{file}", '-o', f"figures/software/{filename}.png"])

# generate pdf documentation
os.chdir(PATH_DOXYGEN)
run(['doxygen'])

os.chdir(PATH_OUTPUT_LATEX)
run(['make', '-s'])

# copy output to `docs` directory
os.chdir(PATH_DOCS)
rmtree(path=f"{PATH_DOXYGEN}/html")
move(src=PATH_OUTPUT_HTML, dst=f"{PATH_DOXYGEN}/html")
copy2(src=f"{PATH_OUTPUT_LATEX}/refman.pdf", dst=PATH_DOCS)
copy2(src=f"{PATH_OUTPUT_LATEX}/refman.log", dst=PATH_DOXYGEN)
