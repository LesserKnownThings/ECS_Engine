cmake_minimum_required(VERSION 3.20)
project(ECS_Engine)

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Editor" "Game")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS "-std=c++20 -g")

set(GLOBAL_SOURCES "")
set(GLOBAL_HEADERS "")

if(CMAKE_BUILD_TYPE STREQUAL "Editor")
	add_compile_definitions(EDITOR_ONLY=1)	
	file(GLOB_RECURSE EDITOR_SOURCES "${CMAKE_SOURCE_DIR}/**/Editor/*.cpp")
	list(APPEND GLOBAL_SOURCES ${EDITOR_SOURCES})
	include_directories(${CMAKE_SOURCE_DIR}/ECS_Engine/Editor)
elseif(CMAKE_BUILD_TYPE STREQUAL "Game")
	add_compile_definitions(EDITOR_ONLY=0)
	file(GLOB_RECURSE EDITOR_HEADERS "${CMAKE_SOURCE_DIR}/**/Editor/*.h")
	list(APPEND GLOBAL_HEADERS ${EDITOR_HEADERS})
endif()

file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/**/Source/Engine/*.cpp")
list(APPEND GLOBAL_SOURCES ${SOURCES})
file(GLOB_RECURSE HEADERS "${CMAKE_SOURCE_DIR}/**/Source/Engine/*.h")
list(APPEND GLOBAL_HEADERS ${HEADERS})

include_directories(${CMAKE_SOURCE_DIR}/Include)
include_directories(${CMAKE_SOURCE_DIR}/Include/physx)
include_directories(${CMAKE_SOURCE_DIR}/ECS_Engine/Source/Engine)

file(GLOB DIRECTORY_LIST LIST_DIRECTORIES TRUE ${CMAKE_SOURCE_DIR}/Include/*)

add_definitions(-DGLEW_NO_GLU)

if (UNIX)
	link_directories(${CMAKE_SOURCE_DIR}/Linking/linux/physx)
endif()

add_executable(ECS_Engine ${GLOBAL_SOURCES} ${GLOBAL_HEADERS})

if(UNIX)	
	find_library(UUID_LIBRARIES NAMES uuid)
	target_link_libraries(ECS_Engine PRIVATE ${UUID_LIBRARIES})
endif()

set(LIBRARY_FILES "")

if(UNIX)
	file(GLOB_RECURSE LIBRARY_FILES ${CMAKE_SOURCE_DIR}/Linking/*.so.*)
elseif(WIN32)
	file(GLOB_RECURSE LIBRARY_FILES ${CMAKE_SOURCE_DIR}/Linking/*.lib ${CMAKE_SOURCE_DIR}/Linking/*.dll)
endif()

message(STATUS "Lib files ==> ${LIBRARY_FILES}")

foreach(lib_file ${LIBRARY_FILES})
if(WIN32)
	get_filename_component(lib_name ${lib_file} NAME_WE)
	target_link_libraries(ECS_Engine PRIVATE ${lib_name})
elseif(UNIX)
	target_link_libraries(ECS_Engine PRIVATE ${lib_file})
endif()
endforeach()

target_link_libraries(ECS_Engine
	PRIVATE
	PhysXExtensions_static_64
    PhysX_static_64
    PhysXPvdSDK_static_64
    PhysXCommon_static_64
    PhysXFoundation_static_64
	
    pthread
    dl
)
