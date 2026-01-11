project(Sandbox)

# 收集源文件
file(GLOB_RECURSE SANDBOX_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SANDBOX_SOURCES})

# 链接引擎库
target_link_libraries(${PROJECT_NAME} PRIVATE Misaka)

# 复制资源文件到输出目录
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)
