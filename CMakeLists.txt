cmake_minimum_required(VERSION 3.25)

project(Misaka3D)
set(CXX_STANDARD 21)

include_directories(includes src src/vendor src/core src/examples)
link_libraries("${PROJECT_SOURCE_DIR}/libs/GLFW/libglfw3.a")
link_libraries("${PROJECT_SOURCE_DIR}/libs/Assimp/libassimp.a")
link_libraries("${PROJECT_SOURCE_DIR}/libs/zlib/libzlibstatic.a")


aux_source_directory(./src src)

file(GLOB_RECURSE scc src/**/*.cpp)
file(GLOB_RECURSE spp src/**/*.hpp)

add_executable(Misaka3D ${src} ${scc} ${spp})

set(ASSETS_SOURCE_DIR "${PROJECT_SOURCE_DIR}/assets")
set(ASSETS_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/assets")

add_custom_command(
    TARGET Misaka3D POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${ASSETS_SOURCE_DIR}"
            "${ASSETS_BINARY_DIR}"
    COMMENT "Copying assets to runtime output folder"
)