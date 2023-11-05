#**************************************************************************************************
# File:         /docs/doc_gen.py
# Description:  Generates images and documentation
#**************************************************************************************************
import os
from shutil import copy2, move, rmtree
from subprocess import run

print("Generating documentation...")

'''NOTE: this script should be run from the project's root directory'''
PATH_ROOT = os.getcwd()

# generate images
PATH_DOCS = os.path.join(PATH_ROOT, "docs")
PATH_DOXYGEN = os.path.join(PATH_DOCS, "doxygen")
PATH_FIG_OUTPUT = os.path.join(PATH_DOCS, "figures", "software")

for file in os.listdir(PATH_DOXYGEN):
    filename, file_extension = os.path.splitext(file)
    if file_extension == '.dot':
        input_file_path = os.path.join(PATH_DOXYGEN, file)
        output_file_path = os.path.join(PATH_FIG_OUTPUT, filename) + ".png"
        run(['dot', '-Tpng', input_file_path, '-o', output_file_path])

# generate Doxygen documentation
PATH_OUTPUT = os.path.join(PATH_ROOT, "build", "docs")
PATH_OUTPUT_HTML = os.path.join(PATH_OUTPUT, "html")
PATH_OUTPUT_LATEX = os.path.join(PATH_OUTPUT, "latex")

os.chdir(PATH_DOXYGEN)
run(['doxygen'])

# move html output
rmtree(path=f"{PATH_DOXYGEN}/html")
move(src=PATH_OUTPUT_HTML, dst=f"{PATH_DOXYGEN}/html")

# generate pdf documentation and move to correct directory
os.chdir(PATH_OUTPUT_LATEX)
run(['make', '-s'])

copy2(src=f"{PATH_OUTPUT_LATEX}/refman.pdf", dst=PATH_DOCS)
copy2(src=f"{PATH_OUTPUT_LATEX}/refman.log", dst=PATH_DOXYGEN)

print("...done.")
