cmake_minimum_required (VERSION 3.5)

project(RenderToy_Core)

add_definitions("-DUNICODE" "-D_UNICODE" "-DNOMINMAX")
add_compile_definitions("RT_ROOT_DIR=${CMAKE_SOURCE_DIR}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS") 

add_executable(${PROJECT_NAME} WIN32
    src/core/Main.cpp
    src/core/IObject.cpp

    src/core/RenderToy/RenderToy.cpp

    src/core/AssetMngr/AssetMngr.cpp

    src/core/RenderComponent/RenderComponent.cpp

    src/core/LogicalComponent/LogicalComponent.cpp

    src/core/Editor/Editor.cpp

    src/core/ObjectMngr/ObjectMngr.cpp
    src/core/ObjectMngr/BasicObject.cpp

    src/core/PropertyMngr/Material.cpp
    src/core/PropertyMngr/Transform.cpp
    src/core/PropertyMngr/PropertyMngr.cpp
    src/core/PropertyMngr/Mesh.cpp

    src/core/Pass/render/PhongPass.cpp
    src/core/Pass/render/ShadowPass.cpp
    src/core/Pass/render/SsaoPass.cpp

    src/core/Pass/logical/UpdatePass.cpp
    src/core/Pass/logical/TransUpdatePass.cpp
    src/core/Pass/logical/MatUpdatePass.cpp
    src/core/Pass/logical/MeshUpdatePass.cpp
    src/core/Pass/logical/ShadowPrePass.cpp
    src/core/Pass/logical/SsaoPrePass.cpp

    src/core/Pass/logical/shadow/ShadowMap.cpp
    src/core/Pass/logical/ssao/Ssao.cpp

    src/core/Utility/Macro.cpp
    src/core/Utility/GlobalParam.cpp
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
    PUBLIC imgui
    PUBLIC d3d12.lib
    PUBLIC dxgi.lib
    PUBLIC d3dcompiler.lib
)
