cmake_minimum_required(VERSION 3.28)
project(xide C)

set(CMAKE_C_STANDARD 23)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
# set(CMAKE_VERBOSE_MAKEFILE ON CACHE INTERNAL "")

set(XIDE_USE_VULKAN OFF)

if (XIDE_USE_VULKAN)
    find_package(Vulkan REQUIRED)
    include_directories()
    set(XIDE_GL ${Vulkan_LIBRARIES})
    include_directories(${Vulkan_INCLUDE_DIRS})
    add_compile_definitions("XIDE_USE_VULKAN")
else ()
    find_package(OpenGL REQUIRED)
    add_library(glad STATIC glad/src/glad.c)
    include_directories(glad/include)
    set(XIDE_GL glad)
endif ()

add_compile_definitions("GLFW_WINDOW_DECORATION_USING_ENUM")
add_subdirectory(glfw)
include_directories(glfw/include)

add_subdirectory(freetype)
include_directories(freetype/include)

#add_compile_options("-O3")
add_compile_options("-Werror" "-Wall" "-Wextra")
add_compile_options("-ftree-vectorize" "-msse4" "-mavx2")

aux_source_directory(meman MEMAN_SRC)
add_library(meman STATIC ${MEMAN_SRC})

add_subdirectory(xglm)
include_directories(xglm/src)
link_libraries(xglm)

include_directories(
        runtime
        widgets
        style
        computational-geometry
        meman
)

link_libraries(meman)

aux_source_directory(computational-geometry COM_GEO_SRC)
add_library(com-geo STATIC ${COM_GEO_SRC})

aux_source_directory(widgets UI_SRC)
add_library(widgets STATIC ${UI_SRC})

aux_source_directory(runtime RT_SRC)
add_library(runtime STATIC ${RT_SRC})
target_link_libraries(runtime PRIVATE ${XIDE_GL} com-geo widgets freetype)

aux_source_directory(application APP_SRC)
add_executable(xide ${APP_SRC})
target_link_libraries(xide PRIVATE
        ${OPENGL_LIBRARIES} ${XIDE_GL} glfw com-geo widgets runtime)
