project (imgui)

add_library(${PROJECT_NAME} 
    src/imgui.cpp
    src/imgui_demo.cpp
    src/imgui_draw.cpp
    src/imgui_impl_dx12.cpp
    src/imgui_impl_win32.cpp
    src/imgui_tables.cpp
    src/imgui_widgets.cpp
)

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)