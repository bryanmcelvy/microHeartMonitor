#**************************************************************************************************
# File:         /docs/doc_gen.py
# Description:  Generates images and pdf documentation
#**************************************************************************************************
import os
import shutil
from subprocess import run

PROJECT_ROOT_DIR = os.getcwd()
LATEX_OUTPUT_DIR = f"{PROJECT_ROOT_DIR}/build/docs/latex"
PATH_DOCS = f"{PROJECT_ROOT_DIR}/docs"
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

os.chdir(LATEX_OUTPUT_DIR)
run(['make', '-s'])

# copy new pdf and reflog to `docs` directory
os.chdir(PATH_DOCS)
shutil.copy2(src=f"{LATEX_OUTPUT_DIR}/refman.pdf", dst=PATH_DOCS)
shutil.copy2(src=f"{LATEX_OUTPUT_DIR}/refman.log", dst=PATH_DOXYGEN)
