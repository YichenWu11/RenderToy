cmake_minimum_required (VERSION 3.5)

project(RenderToy_Core)

add_definitions("-DUNICODE" "-D_UNICODE")

add_executable(${PROJECT_NAME} WIN32
    src/core/Main.cpp
)
add_link_options(${PROJECT_NAME} PRIVATE -mwindows)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

message(STATUS ${PROJECT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME}
    PUBLIC Chen::CDX12
    PUBLIC Chen::CDP
    PUBLIC d3d12.lib
    PUBLIC dxgi.lib
    PUBLIC d3dcompiler.lib
)
