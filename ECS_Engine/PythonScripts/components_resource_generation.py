import glob
import os

def find_header_files(directory):
    h_files = glob.glob(os.path.join(directory, '**/*.h'), recursive=True)
    hpp_files = glob.glob(os.path.join(directory, '**/*.hpp'), recursive=True)
    
    return h_files + hpp_files

project_directory = '../Source'
header_files = find_header_files(project_directory)

for file in header_files:
    