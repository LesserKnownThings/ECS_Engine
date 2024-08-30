cmake_minimum_required(VERSION 3.20)
project(ECS_Engine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/**/Source/*.cpp")
file(GLOB_RECURSE HEADERS "${CMAKE_SOURCE_DIR}/**/Source/*.h")

# These are added manually because they're not part of the auto build
include_directories(${CMAKE_SOURCE_DIR}/ECS_Engine/Source/Engine/imgui)
include_directories(${CMAKE_SOURCE_DIR}/ECS_Engine/GlobalIncludes)
include_directories(${CMAKE_SOURCE_DIR}/ECS_Engine/Source/Engine)

link_directories(${CMAKE_SOURCE_DIR}/Libs)

add_executable(ECS_Engine ${SOURCES} ${HEADERS})

file(GLOB_RECURSE ALL_HEADERS LIST_DIRECTORIES FALSE ${CMAKE_SOURCE_DIR}/Include/*)

set(HEADER_DIRS "")

foreach(header_file IN LISTS ALL_HEADERS)
    get_filename_component(header_dir ${header_file} DIRECTORY)
    list(FIND HEADER_DIRS ${header_dir} dir_index)
    if(dir_index EQUAL -1)
        list(APPEND HEADER_DIRS ${header_dir})
    endif()
endforeach()

include_directories(${HEADER_DIRS})

message(STATUS "Library directories: ${CMAKE_SOURCE_DIR}/Libs")

file(GLOB_RECURSE LIBRARY_FILES ${CMAKE_SOURCE_DIR}/Libs/*.a ${CMAKE_SOURCE_DIR}/Libs/*.lib ${CMAKE_SOURCE_DIR}/Libs/*.so ${CMAKE_SOURCE_DIR}/Libs/*.dll)

foreach(lib_file ${LIBRARY_FILES})
    get_filename_component(lib_name ${lib_file} NAME_WE)
    target_link_libraries(ECS_Engine PRIVATE ${lib_name})
endforeach()

target_link_libraries(ECS_Engine PRIVATE opengl32)
