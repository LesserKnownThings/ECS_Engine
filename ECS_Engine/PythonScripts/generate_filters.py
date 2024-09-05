import os
import uuid
import xml.etree.ElementTree as ET

namespaces = {'msbuild': 'http://schemas.microsoft.com/developer/msbuild/2003'}

# Base path of your project
base_path = "Source\\Engine"

public_filter = f"Public"
private_filter = f"Private"

# Output filters file
filters_file = "ECS_Engine.vcxproj.filters"

# Generate a unique identifier for each filter
def generate_guid():
    return str(uuid.uuid4()).upper()

# Create a filter element
def create_filter_element(name):
    filter_elem = ET.Element('Filter', {'Include': name})
    unique_id_elem = ET.SubElement(filter_elem, 'UniqueIdentifier')
    unique_id_elem.text = f'{{{generate_guid()}}}'
    return filter_elem

# Create a file element (ClInclude or ClCompile)
def create_file_element(file_path, filter_name):
    file_extension = os.path.splitext(file_path)[1]
    if file_extension in ['.h', '.hpp']:
        element_name = 'ClInclude'
    elif file_extension in ['.cpp', '.c']:
        element_name = 'ClCompile'
    else:
        return None

    file_elem = ET.Element(element_name, {'Include': file_path})
    filter_elem = ET.SubElement(file_elem, 'Filter')
    filter_elem.text = filter_name
    return file_elem

def add_filters(parent, dir_path, rel_path=''):
    parent.append(create_filter_element(f"{public_filter}{rel_path}"))
    parent.append(create_filter_element(f"{private_filter}{rel_path}"))
    
    for item in os.listdir(dir_path):        
        full_path = os.path.join(dir_path, item)
        if(os.path.isdir(full_path)):
            add_filters(parent, full_path, f"{rel_path}\\{item}")



def add_files(parent, dir_path, rel_path=''):
    for item in os.listdir(dir_path):        
        full_path = os.path.join(dir_path, item)
        
        in_between = "";
            
        if rel_path != "":
            in_between = "\\"
                
        if os.path.isfile(full_path):          
            if item.endswith('.h'):
                file_element = create_file_element(full_path, f"{public_filter}{in_between}{rel_path}")
            else:
                file_element = create_file_element(full_path, f"{private_filter}{in_between}{rel_path}")

            if file_element is not None:
                parent.append(file_element)
        elif os.path.isdir(full_path):
            
            add_files(parent, full_path, f'{rel_path}{in_between}{item}')

def add_third_party_files(parent, dir_path, rel_path=""):
    for item in os.listdir(dir_path):        
        full_path = os.path.join(dir_path, item)
        
        in_between = "";
            
        if rel_path != "":
            in_between = "\\"
                
        if os.path.isfile(full_path):       
            file_element = create_file_element(f"..\\{full_path}", "ThirdParty")

            if file_element is not None:
                parent.append(file_element)
        elif os.path.isdir(full_path):            
            add_third_party_files(parent, full_path, f'{rel_path}{in_between}{item}')
        
        

def create_vcxproj_filters():
    ET.register_namespace("", "http://schemas.microsoft.com/developer/msbuild/2003")
    
    if os.path.exists(filters_file):
        tree = ET.parse(filters_file)
        root = tree.getroot()
    else:
        root = ET.Element("Project", {"ToolsVersion": "4.0", "xmlns": "http://schemas.microsoft.com/developer/msbuild/2003"})        

    item_groups = root.findall('.//msbuild:ItemGroup', namespaces=namespaces)
    
    for item_group in item_groups:
        root.remove(item_group)

    filters_group = ET.SubElement(root, 'ItemGroup')
    add_filters(filters_group, base_path)
    
    filters_group.append(create_filter_element("ThirdParty"))
    
    files_group = ET.SubElement(root, "ItemGroup")
    add_files(files_group, base_path)
    
    os.chdir("..")
    
    add_third_party_files(files_group, "Include")
   
    os.chdir("ECS_Engine")

    tree = ET.ElementTree(root)
    ET.indent(tree, space="  ", level=0)
  
    tree.write(filters_file, xml_declaration=True, encoding="utf-8")

if __name__ == '__main__':
    create_vcxproj_filters()