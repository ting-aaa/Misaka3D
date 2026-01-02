cmake_minimum_required(VERSION 3.16)
project(Misaka3D)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 添加子项目
add_subdirectory(Misaka)
add_subdirectory(Sandbox)
