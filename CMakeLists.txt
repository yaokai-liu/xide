cmake_minimum_required(VERSION 3.28)
project(xide C)

set(CMAKE_C_STANDARD 23)
add_compile_options("-O3")
add_compile_options("-ftree-vectorize" "-msse3")
add_compile_definitions("nullptr=NULL")


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_VERBOSE_MAKEFILE ON CACHE INTERNAL "")

find_package(OpenGL REQUIRED)

aux_source_directory(meman MEMAN_SRC)
add_library(meman STATIC ${MEMAN_SRC})

add_library(glad STATIC glad/src/glad.c)
include_directories(glad/include)

add_subdirectory(glfw)
include_directories(glfw/include)

add_subdirectory(freetype)
include_directories(freetype/include)

include_directories(
        runtime
        components
        style
        com-geo
        meman
)

link_libraries(meman)

aux_source_directory(runtime RT_SRC)
add_library(runtime STATIC ${RT_SRC})
target_link_libraries(runtime PRIVATE glad com-geo components freetype)

aux_source_directory(components UI_SRC)
add_library(components STATIC ${UI_SRC})

aux_source_directory(com-geo COM_GEO_SRC)
add_library(com-geo STATIC ${COM_GEO_SRC})

add_executable(xide main.c)
target_link_libraries(xide PRIVATE
        ${OPENGL_LIBRARIES} glad glfw com-geo components runtime)
