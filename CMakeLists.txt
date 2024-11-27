cmake_minimum_required(VERSION 3.28)
project(xide C)

set(CMAKE_C_STANDARD 11)
add_compile_options("-O0")
add_compile_definitions("nullptr=NULL")

#if(CMAKE_)
#add_compile_options("/Wall" "/WX")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)

add_library(glad STATIC glad/src/glad.c)
include_directories(glad/include)

add_subdirectory(glfw)
include_directories(glfw/include)

include_directories(
        runtime
        components
        style
        com-geo
)

aux_source_directory(runtime RT_SRC)
add_library(runtime STATIC ${RT_SRC})
target_link_libraries(runtime PRIVATE com-geo components)

aux_source_directory(components UI_SRC)
add_library(components STATIC ${UI_SRC})

aux_source_directory(style STYLE_SRC)
add_library(style STATIC ${STYLE_SRC})

aux_source_directory(com-geo COM_GEO_SRC)
add_library(com-geo STATIC ${COM_GEO_SRC})

add_executable(xide main.c)
target_link_libraries(xide PRIVATE
        opengl32 glad glfw com-geo components runtime style)
