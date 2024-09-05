import xml.etree.ElementTree as ET
import os

def make_relative(path, base_dir):
    return os.path.relpath(path, base_dir)

def modify_vcxproj(vcxproj_path, base_dir):
    tree = ET.parse(vcxproj_path)
    root = tree.getroot()

    namespace = {'ns': 'http://schemas.microsoft.com/developer/msbuild/2003'}
    ET.register_namespace("", "http://schemas.microsoft.com/developer/msbuild/2003")

    for cl_compile in root.findall(".//ns:ClCompile", namespace):
        original_path = cl_compile.get('Include')
        if original_path:
            relative_path = make_relative(original_path, base_dir)
            cl_compile.set('Include', relative_path)

    for cl_include in root.findall(".//ns:ClInclude", namespace):
        original_path = cl_include.get('Include')
        if original_path:
            relative_path = make_relative(original_path, base_dir)
            cl_include.set('Include', relative_path)

    tree.write(vcxproj_path, encoding="utf-8", xml_declaration=True)

if __name__ == "__main__":    
    vcxproj_file = "ECS_Engine.vcxproj"

    base_directory = os.path.dirname(vcxproj_file)
    
    modify_vcxproj(vcxproj_file, base_directory)
    