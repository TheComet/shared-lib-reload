cmake_minimum_required (VERSION 3.21)

project (shared-lib-reload
    VERSION 0.0.1
    LANGUAGES C)

add_library (shared SHARED
    "shared/shared.c")
target_include_directories (shared
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/shared>)
target_compile_definitions (shared
    PRIVATE SHARED_BUILDING)
set_target_properties (shared PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    PREFIX "")

if (WIN32)
    target_link_libraries (shared PRIVATE ws2_32)
endif ()

add_executable (app
    "app/main.c")
target_include_directories (app
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/shared>)
set_target_properties (app PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
