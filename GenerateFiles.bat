@echo off
set CURRENT_DIR=%CD%
cd ECS_Engine
cmake ..
move /y ECS_Engine.sln CURRENT_DIR
python PythonScripts/absolute_to_relative_path.py
python PythonScripts/generate_filters.py
exit