project(Misaka)

# 收集引擎源文件
file(GLOB_RECURSE MISAKA_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
file(GLOB_RECURSE MISAKA_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

# 第三方源码
set(VENDOR_SOURCES
    ${CMAKE_SOURCE_DIR}/vendor/glad/glad.c
    ${CMAKE_SOURCE_DIR}/vendor/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/vendor/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/vendor/imgui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/vendor/imgui/imgui_widgets.cpp
    ${CMAKE_SOURCE_DIR}/vendor/imgui/imgui_impl_glfw.cpp
    ${CMAKE_SOURCE_DIR}/vendor/imgui/imgui_impl_opengl3.cpp
)

# 创建静态库
add_library(${PROJECT_NAME} STATIC 
    ${MISAKA_SOURCES} 
    ${MISAKA_HEADERS}
    ${VENDOR_SOURCES}
)

# 头文件搜索路径
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}              # 使 #include <Misaka/Misaka.h> 可用
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/includes
    ${CMAKE_SOURCE_DIR}/vendor
    ${CMAKE_SOURCE_DIR}/vendor/imgui
)

# 库文件搜索路径
target_link_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/libs/GLFW
    ${CMAKE_SOURCE_DIR}/libs/Assimp
    ${CMAKE_SOURCE_DIR}/libs/zlib
)

# 链接库 (MinGW 使用 .a 库)
target_link_libraries(${PROJECT_NAME} PUBLIC
    glfw3
    assimp
    zlibstatic
    opengl32
)

# 预编译宏
target_compile_definitions(${PROJECT_NAME} PUBLIC
    MISAKA_ENGINE
    $<$<CONFIG:Debug>:MISAKA_DEBUG>
)
