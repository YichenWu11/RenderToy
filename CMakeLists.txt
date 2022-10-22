cmake_minimum_required (VERSION 3.5)

project(RenderToy)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILECOMMANDS ON)

# Add sub directories
add_subdirectory(CDP)
add_subdirectory(CDX12)
add_subdirectory(RenderToy)
