cmake_minimum_required (VERSION 3.5)

project(RenderToy)

set(RT_ROOT_DIR "${CMAKE_SOURCE_DIR}")
message(STATUS ${RT_ROOT_DIR})
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS") 
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILECOMMANDS ON)

# Add sub directories
add_subdirectory(CDP)
add_subdirectory(CDX12)
add_subdirectory(RenderToy)
add_subdirectory(3rdparty/imgui)
