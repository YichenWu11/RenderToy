cmake_minimum_required (VERSION 3.5)

set(RT_ROOT_DIR "${CMAKE_SOURCE_DIR}")
message(STATUS ${RT_ROOT_DIR})

if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS") 
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILECOMMANDS ON)
# 静态库输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/lib)
# 动态库输出目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)
# 可执行文件输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)

project(RenderToy)

add_definitions(-DUNICODE)

if (WIN32)
    add_definitions(-DNOMINMAX -D_USE_MATH_DEFINES)
endif()

# Add sub directories
add_subdirectory(CDP)
add_subdirectory(CDX12)
add_subdirectory(RenderToy)
add_subdirectory(3rdparty/imgui)
add_subdirectory(3rdparty/rapidjson)
