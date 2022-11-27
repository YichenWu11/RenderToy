project (rapidjson)

add_library(${PROJECT_NAME} INTERFACE)

# message(STATUS ${PROJECT_SOURCE_DIR})

target_include_directories(rapidjson
    INTERFACE ${PROJECT_SOURCE_DIR}/include
)
