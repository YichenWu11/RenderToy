# Set the project name
project (CDP)

add_library(${PROJECT_NAME} INTERFACE)
add_library(Chen::CDP ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    INTERFACE
        ${PROJECT_SOURCE_DIR}/include
)
