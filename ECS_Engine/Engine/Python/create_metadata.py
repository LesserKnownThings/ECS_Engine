import os
import re

def contains_define(fileName):
    try:
        with open(fileName, 'r') as file:
            content = file.read()

            if re.search(r'#define\s+CREATE_METADATA\s*\(".*?"\)'):
                return True
            
    except (IOError, UnicodeDecodeError) as e:
        print(f"Error reading {fileName} : {e}")
    return False

def search_dir(directory):
    for root, _, files in os.walk(directory):
        for file in files:
            filepath = os.path.join(root, file)
            if contains_define(filepath):
                print(f"File with #define CREATE_METADATA found: {filepath}")
