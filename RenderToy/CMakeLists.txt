cmake_minimum_required (VERSION 3.5)

project(RenderToy_Core)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS") 

file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/core/*.cpp include/*.h)

add_executable(${PROJECT_NAME} ${srcs})

# win32 gui program
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    WIN32_EXECUTABLE ON
)

target_compile_definitions(RenderToy_Core PUBLIC RT_ROOT_DIR=${CMAKE_SOURCE_DIR})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

message(STATUS ${PROJECT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME}
    PUBLIC Chen::CDX12
    PUBLIC Chen::CDP
    PUBLIC imgui
    PUBLIC d3d12.lib
    PUBLIC dxgi.lib
    PUBLIC d3dcompiler.lib
)
