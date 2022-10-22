cmake_minimum_required (VERSION 3.5)

project(RenderToy_Core)

add_executable(${PROJECT_NAME} 
    Main.cpp
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC Chen::CDX12
    PUBLIC Chen::CDP
    PUBLIC d3d12.lib
    PUBLIC dxgi.lib
    PUBLIC d3dcompiler.lib
)
