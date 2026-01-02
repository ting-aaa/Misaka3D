# Misaka3D 重构计划文档

## 📋 概述

本文档描述了 Misaka3D 渲染引擎的代码重构计划，主要目标是：
1. **Library 化架构** - 将引擎核心封装为独立静态库，与应用程序分离
2. **规范化目录结构** - 按照功能模块分类存放代码
3. **头文件与实现分离** - 将 `.h` 和 `.cpp` 文件分离，提高编译效率和代码可维护性
4. **统一命名空间** - 使用 `Misaka` 命名空间包裹所有引擎代码

---

## 📁 当前目录结构分析

```
src/
├── glad.c
├── main.cpp
├── StbImageWarpper.cpp
├── leave3D/
│   ├── AssetManager.h
│   ├── Camera3D.h
│   ├── Geometry.h
│   ├── Mesh.h
│   ├── ModelImporter.h
│   ├── ModelPrefab.h
│   ├── Object3D.h
│   ├── ObjectContainer3D.h
│   ├── Renderer.h
│   ├── Scene3D.h
│   ├── SceneBuilder.h
│   ├── Texture.h
│   ├── View3D.h
│   ├── light/
│   │   ├── DirectionalLight.h
│   │   ├── LightBase.h
│   │   └── PointLight.h
│   ├── material/
│   │   ├── CommonMaterial.h
│   │   ├── MaterialBase.h
│   │   └── Shader.h
│   └── resources/
│       ├── IResource.h
│       └── TextureLoader.h
```

### 当前问题

| 问题 | 描述 |
|------|------|
| 🔴 **Header-only 设计** | 所有类都在 `.h` 文件中实现，没有 `.cpp` 分离 |
| 🔴 **目录混乱** | `Renderer`、`Camera3D`、`Scene3D` 等不同类型的类混放在同一目录 |
| 🔴 **职责不清** | `ObjectContainer3D` 既是场景节点又包含渲染逻辑 |
| 🟡 **命名不一致** | 部分使用 `3D` 后缀，部分不使用 |
| 🟡 **资源系统分散** | `TextureLoader` 在 resources/ 目录，但 `ModelImporter` 在根目录 |
| 🔴 **缺少命名空间** | 没有命名空间，容易与其他库产生命名冲突 |
| 🔴 **非 Library 结构** | 引擎代码与应用程序混在一起，无法独立编译为库 |

---

## 🏗️ Library 化架构设计

### 目标结构

```
Misaka3D/
├── CMakeLists.txt                 # 顶层 CMake
├── assets/                        # 资源文件
│   ├── shader/
│   ├── model/
│   └── texture/
│
├── Misaka/                        # 🔶 引擎库 (静态库)
│   ├── CMakeLists.txt             # 引擎库 CMake
│   ├── Misaka.h                   # 统一头文件 (用户只需 include 这一个)
│   └── src/
│       ├── core/
│       ├── renderer/
│       ├── scene/
│       ├── entity/
│       ├── camera/
│       ├── material/
│       └── resources/
│
├── Sandbox/                       # 🔷 示例应用程序
│   ├── CMakeLists.txt
│   └── src/
│       └── main.cpp
│
├── includes/                      # 第三方头文件
│   ├── glad/
│   ├── GLFW/
│   ├── assimp/
│   └── stb_image.h
│
├── libs/                          # 第三方库文件
│   ├── GLFW/
│   ├── Assimp/
│   └── zlib/
│
└── vendor/                        # 需要编译的第三方源码
    ├── glad/
    │   └── glad.c
    ├── imgui/
    └── glm/
```

### CMake 配置

**顶层 CMakeLists.txt**
```cmake
cmake_minimum_required(VERSION 3.16)
project(Misaka3D)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加子项目
add_subdirectory(Misaka)
add_subdirectory(Sandbox)
```

**Misaka/CMakeLists.txt (引擎库)**
```cmake
project(Misaka)

# 收集源文件
file(GLOB_RECURSE MISAKA_SOURCES "src/*.cpp")
file(GLOB_RECURSE MISAKA_HEADERS "src/*.h")

# 添加第三方源码
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
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/includes
    ${CMAKE_SOURCE_DIR}/vendor
    ${CMAKE_SOURCE_DIR}/vendor/imgui
)

# 链接库
target_link_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/libs/GLFW
    ${CMAKE_SOURCE_DIR}/libs/Assimp
    ${CMAKE_SOURCE_DIR}/libs/zlib
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    glfw3
    assimp-vc143-mt
    zlibstatic
    opengl32
)

# 预编译宏
target_compile_definitions(${PROJECT_NAME} PUBLIC
    MISAKA_ENGINE
    $<$<CONFIG:Debug>:MISAKA_DEBUG>
)
```

**Sandbox/CMakeLists.txt (应用程序)**
```cmake
project(Sandbox)

file(GLOB_RECURSE SANDBOX_SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} ${SANDBOX_SOURCES})

# 链接引擎库
target_link_libraries(${PROJECT_NAME} PRIVATE Misaka)

# 复制资源文件
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)
```

---

## 🏷️ 命名空间设计

### 命名空间层级

```cpp
namespace Misaka {
    // 核心模块
    namespace Core { }
    
    // 渲染模块  
    namespace Renderer { }
    
    // 所有类直接放在 Misaka 命名空间下
    class Scene3D;
    class Camera3D;
    class Mesh;
    // ...
}
```

### 使用示例

```cpp
// 用户代码
#include <Misaka/Misaka.h>

int main() {
    // 方式1：使用完整命名空间
    Misaka::Scene3D* scene = new Misaka::Scene3D();
    Misaka::Camera3D* camera = new Misaka::Camera3D(45.0f, 16.0f/9.0f, 0.1f, 100.0f);
    
    // 方式2：使用 using namespace
    using namespace Misaka;
    Renderer* renderer = new Renderer();
    
    // 方式3：使用类型别名
    using MScene = Misaka::Scene3D;
    MScene* scene2 = new MScene();
    
    return 0;
}
```

---

## 📁 引擎库目录结构

```
Misaka/
├── CMakeLists.txt
├── Misaka.h                       # 统一入口头文件
└── src/
    ├── core/                      # 核心基础设施
    │   └── Base.h                 # 通用宏定义、类型别名
    │
    ├── renderer/                  # 渲染器模块
    │   ├── Renderer.h / .cpp
    │   ├── View3D.h / .cpp
    │   ├── Shader.h / .cpp
    │   ├── Geometry.h / .cpp
    │   └── Texture.h / .cpp
    │
    ├── scene/                     # 场景管理模块
    │   ├── Scene3D.h / .cpp
    │   └── SceneBuilder.h / .cpp
    │
    ├── entity/                    # 实体模块
    │   ├── Object3D.h / .cpp
    │   ├── ObjectContainer3D.h / .cpp
    │   ├── Mesh.h / .cpp
    │   ├── ModelPrefab.h / .cpp
    │   └── light/
    │       ├── LightBase.h / .cpp
    │       ├── DirectionalLight.h / .cpp
    │       └── PointLight.h / .cpp
    │
    ├── camera/                    # 相机模块
    │   └── Camera3D.h / .cpp
    │
    ├── material/                  # 材质模块
    │   ├── MaterialBase.h / .cpp
    │   ├── CommonMaterial.h / .cpp
    │   └── ColorMaterial.h / .cpp
    │
    └── resources/                 # 资源管理模块
        ├── IResource.h / .cpp
        ├── AssetManager.h / .cpp
        ├── TextureLoader.h / .cpp
        └── ModelImporter.h / .cpp
```

### 统一头文件 Misaka.h

```cpp
#pragma once

// ============================================
// Misaka3D Engine - 统一头文件
// 用户只需 #include <Misaka/Misaka.h>
// ============================================

// Core
#include "src/core/Base.h"

// Resources
#include "src/resources/IResource.h"
#include "src/resources/AssetManager.h"
#include "src/resources/TextureLoader.h"
#include "src/resources/ModelImporter.h"

// Renderer
#include "src/renderer/Shader.h"
#include "src/renderer/Geometry.h"
#include "src/renderer/Texture.h"
#include "src/renderer/Renderer.h"
#include "src/renderer/View3D.h"

// Entity
#include "src/entity/Object3D.h"
#include "src/entity/ObjectContainer3D.h"
#include "src/entity/Mesh.h"
#include "src/entity/ModelPrefab.h"
#include "src/entity/light/LightBase.h"
#include "src/entity/light/DirectionalLight.h"
#include "src/entity/light/PointLight.h"

// Scene
#include "src/scene/Scene3D.h"
#include "src/scene/SceneBuilder.h"

// Camera
#include "src/camera/Camera3D.h"

// Material
#include "src/material/MaterialBase.h"
#include "src/material/CommonMaterial.h"
#include "src/material/ColorMaterial.h"
```

---

## 🔄 类与文件位置映射

> **注意：保留原有类名，添加 `Misaka` 命名空间，调整目录位置并分离头文件/实现**

| 类名 | 新位置 | 命名空间 |
|------|--------|----------|
| `Object3D` | `Misaka/src/entity/Object3D.h/.cpp` | `Misaka::Object3D` |
| `ObjectContainer3D` | `Misaka/src/entity/ObjectContainer3D.h/.cpp` | `Misaka::ObjectContainer3D` |
| `Scene3D` | `Misaka/src/scene/Scene3D.h/.cpp` | `Misaka::Scene3D` |
| `Camera3D` | `Misaka/src/camera/Camera3D.h/.cpp` | `Misaka::Camera3D` |
| `View3D` | `Misaka/src/renderer/View3D.h/.cpp` | `Misaka::View3D` |
| `Mesh` | `Misaka/src/entity/Mesh.h/.cpp` | `Misaka::Mesh` |
| `Geometry` | `Misaka/src/renderer/Geometry.h/.cpp` | `Misaka::Geometry` |
| `Renderer` | `Misaka/src/renderer/Renderer.h/.cpp` | `Misaka::Renderer` |
| `Shader` | `Misaka/src/renderer/Shader.h/.cpp` | `Misaka::Shader` |
| `Texture` | `Misaka/src/renderer/Texture.h/.cpp` | `Misaka::Texture` |
| `MaterialBase` | `Misaka/src/material/MaterialBase.h/.cpp` | `Misaka::MaterialBase` |
| `CommonMaterial` | `Misaka/src/material/CommonMaterial.h/.cpp` | `Misaka::CommonMaterial` |
| `ColorMaterial` | `Misaka/src/material/ColorMaterial.h/.cpp` | `Misaka::ColorMaterial` |
| `LightBase` | `Misaka/src/entity/light/LightBase.h/.cpp` | `Misaka::LightBase` |
| `DirectionalLight` | `Misaka/src/entity/light/DirectionalLight.h/.cpp` | `Misaka::DirectionalLight` |
| `PointLight` | `Misaka/src/entity/light/PointLight.h/.cpp` | `Misaka::PointLight` |
| `AssetManager` | `Misaka/src/resources/AssetManager.h/.cpp` | `Misaka::AssetManager` |
| `ModelImporter` | `Misaka/src/resources/ModelImporter.h/.cpp` | `Misaka::ModelImporter` |
| `ModelPrefab` | `Misaka/src/entity/ModelPrefab.h/.cpp` | `Misaka::ModelPrefab` |
| `IResource` | `Misaka/src/resources/IResource.h/.cpp` | `Misaka::IResource` |
| `TextureLoader` | `Misaka/src/resources/TextureLoader.h/.cpp` | `Misaka::TextureLoader` |
| `SceneBuilder` | `Misaka/src/scene/SceneBuilder.h/.cpp` | `Misaka::SceneBuilder` |
| `ResourceType` | `Misaka/src/resources/IResource.h` | `Misaka::ResourceType` |
| `LightType` | `Misaka/src/entity/light/LightBase.h` | `Misaka::LightType` |
| `Vertex` | `Misaka/src/renderer/Geometry.h` | `Misaka::Vertex` |
| `PathResolver` | `Misaka/src/resources/AssetManager.h/.cpp` | `Misaka::PathResolver` |

---

## 📝 头文件/实现分离规范

### 原则

1. **头文件 (`.h`)** 只包含：
   - 类声明
   - 内联函数 (inline)
   - 模板实现
   - 常量定义

2. **实现文件 (`.cpp`)** 包含：
   - 类方法实现
   - 静态成员初始化
   
3. 计划中提到要“将 .h 和 .cpp 分离”。 风险：你的 AssetManager 中大量使用了模板函数（如 GetOrLoad<T>）。 原理：C++ 的模板需要在编译期实例化。如果你把 GetOrLoad 的实现移到 AssetManager.cpp，链接器在编译 Sandbox 时会找不到这些函数的定义，报 LNK2019 / Undefined reference 错误。 建议：

    - 不要移动模板实现：GetOrLoad, Find, GetAllAssets 等模板函数的实现必须保留在头文件中（或者放在 .inl 文件中并在头文件末尾 include）。

    - 只移动普通函数：如单例的 Ins(), Clear(), UnloadUnused() 可以移到 cpp。

### 示例：Shader 类分离（带命名空间）

**Misaka/src/renderer/Shader.h**
```cpp
#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

#include "../resources/IResource.h"

namespace Misaka {

class Shader : public IResource {
public:
    static constexpr ResourceType TypeEnum = ResourceType::Shader;
    
    Shader(const char* vertexCode, const char* fragmentCode);
    ~Shader();
    
    // 禁用拷贝
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    
    void Use() const;
    void UnUse() const;
    
    // Uniform 设置
    void SetInt(const char* name, int value) const;
    void SetFloat(const char* name, float value) const;
    void SetBool(const char* name, bool value) const;
    void SetVec3(const char* name, const glm::vec3& value) const;
    void SetVec4(const char* name, const glm::vec4& value) const;
    void SetMat4(const char* name, const glm::mat4& value) const;
    
    ResourceType GetResourceType() const override;
    
private:
    GLuint ID = 0;
    
    void CheckCompileErrors(GLuint shader, const std::string& type);
};

} // namespace Misaka
```

**Misaka/src/renderer/Shader.cpp**
```cpp
#include "Shader.h"
#include <iostream>

namespace Misaka {

Shader::Shader(const char* vertexCode, const char* fragmentCode) {
    // 编译顶点着色器
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexCode, nullptr);
    glCompileShader(vertex);
    CheckCompileErrors(vertex, "VERTEX");
    
    // 编译片段着色器
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentCode, nullptr);
    glCompileShader(fragment);
    CheckCompileErrors(fragment, "FRAGMENT");
    
    // 链接程序
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    CheckCompileErrors(ID, "PROGRAM");
    
    // 清理
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::~Shader() {
    if (ID != 0) {
        glDeleteProgram(ID);
    }
}

void Shader::Use() const {
    glUseProgram(ID);
}

void Shader::UnUse() const {
    glUseProgram(0);
}

void Shader::SetInt(const char* name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name), value);
}

void Shader::SetFloat(const char* name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name), value);
}

void Shader::SetBool(const char* name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name), (int)value);
}

void Shader::SetVec3(const char* name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(ID, name), 1, &value[0]);
}

void Shader::SetVec4(const char* name, const glm::vec4& value) const {
    glUniform4fv(glGetUniformLocation(ID, name), 1, &value[0]);
}

void Shader::SetMat4(const char* name, const glm::mat4& value) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, &value[0][0]);
}

void Shader::CheckCompileErrors(GLuint shader, const std::string& type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "Shader Compile Error (" << type << "): " << infoLog << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "Shader Link Error: " << infoLog << std::endl;
        }
    }
}

ResourceType Shader::GetResourceType() const {
    return ResourceType::Shader;
}

} // namespace Misaka
```

---

## 🏷️ 命名规范

### 文件命名
- 使用 **PascalCase**：`Shader.h`, `ResourceManager.cpp`
- 头文件使用 `.h`，实现文件使用 `.cpp`

### 类命名
- 使用 **PascalCase**：`Shader`, `ResourceManager`
- 基类/接口不使用 `I` 或 `Base` 前缀/后缀

### 成员变量
- 使用 **m_** 前缀 + **camelCase**：`m_position`, `m_shader`
- 或使用 **_** 前缀：`_position`, `_shader` (保持现有风格)

### 函数命名
- 使用 **PascalCase**：`GetPosition()`, `SetRotation()`
- Getter 不使用 `Get` 前缀也可：`Position()` (可选)

### 命名空间
- 使用 `Misaka` 命名空间包裹所有引擎代码

---

## 📅 重构步骤 (分阶段执行)

### 阶段 0：Library 化项目结构 (预计 1 小时)

| 步骤 | 任务 | 优先级 |
|------|------|--------|
| 0.1 | 创建 `Misaka/` 引擎库目录 | 🔴 高 |
| 0.2 | 创建 `Sandbox/` 示例应用目录 | 🔴 高 |
| 0.3 | 移动 `vendor/` 到项目根目录 | 🔴 高 |
| 0.4 | 创建顶层 CMakeLists.txt | 🔴 高 |
| 0.5 | 创建 Misaka/CMakeLists.txt | 🔴 高 |
| 0.6 | 创建 Sandbox/CMakeLists.txt | 🔴 高 |

### 阶段 1：基础设施准备 (预计 1-2 小时)

| 步骤 | 任务 | 优先级 |
|------|------|--------|
| 1.1 | 创建 `Misaka/src/` 下的模块目录 (`core/`, `renderer/`, `scene/`, `entity/`, `camera/`, `material/`, `resources/`) | 🔴 高 |
| 1.2 | 创建 `Misaka/src/core/Base.h` 基础头文件 | 🟡 中 |
| 1.3 | 创建 `Misaka/Misaka.h` 统一入口头文件 | 🔴 高 |

### 阶段 2：资源系统重构 (预计 2-3 小时)

| 步骤 | 任务 | 优先级 |
|------|------|--------|
| 2.1 | `IResource` 分离 .h/.cpp，添加命名空间，移动到 `Misaka/src/resources/` | 🔴 高 |
| 2.2 | `AssetManager` 分离 .h/.cpp，添加命名空间，移动到 `Misaka/src/resources/` | 🔴 高 |
| 2.3 | `TextureLoader` 分离 .h/.cpp，添加命名空间，移动到 `Misaka/src/resources/` | 🔴 高 |
| 2.4 | `ModelImporter` 分离 .h/.cpp，添加命名空间，移动到 `Misaka/src/resources/` | 🔴 高 |

### 阶段 3：渲染器模块重构 (预计 2-3 小时)

| 步骤 | 任务 | 优先级 |
|------|------|--------|
| 3.1 | `Shader` 分离 .h/.cpp，添加命名空间，移动到 `Misaka/src/renderer/` | 🔴 高 |
| 3.2 | `Geometry` 分离 .h/.cpp，添加命名空间，移动到 `Misaka/src/renderer/` | 🔴 高 |
| 3.3 | `Texture` 分离 .h/.cpp，添加命名空间，移动到 `Misaka/src/renderer/` | 🔴 高 |
| 3.4 | `Renderer` 和 `View3D` 分离 .h/.cpp，添加命名空间，移动到 `Misaka/src/renderer/` | 🔴 高 |

### 阶段 4：实体模块重构 (预计 2-3 小时)

| 步骤 | 任务 | 优先级 |
|------|------|--------|
| 4.1 | `Object3D` 分离 .h/.cpp，添加命名空间，移动到 `Misaka/src/entity/` | 🔴 高 |
| 4.2 | `ObjectContainer3D` 分离 .h/.cpp，添加命名空间，移动到 `Misaka/src/entity/` | 🔴 高 |
| 4.3 | `Mesh` 分离 .h/.cpp，添加命名空间，移动到 `Misaka/src/entity/` | 🔴 高 |
| 4.4 | `ModelPrefab` 分离 .h/.cpp，添加命名空间，移动到 `Misaka/src/entity/` | 🔴 高 |
| 4.5 | 光源类分离 .h/.cpp，添加命名空间，移动到 `Misaka/src/entity/light/` | 🔴 高 |

### 阶段 5：场景模块重构 (预计 1-2 小时)

| 步骤 | 任务 | 优先级 |
|------|------|--------|
| 5.1 | `Scene3D` 分离 .h/.cpp，添加命名空间，移动到 `Misaka/src/scene/` | 🔴 高 |
| 5.2 | `SceneBuilder` 分离 .h/.cpp，添加命名空间，移动到 `Misaka/src/scene/` | 🟡 中 |

### 阶段 6：相机模块重构 (预计 1 小时)

| 步骤 | 任务 | 优先级 |
|------|------|--------|
| 6.1 | `Camera3D` 分离 .h/.cpp，添加命名空间，移动到 `Misaka/src/camera/` | 🔴 高 |

### 阶段 7：材质模块重构 (预计 1-2 小时)

| 步骤 | 任务 | 优先级 |
|------|------|--------|
| 7.1 | `MaterialBase` 分离 .h/.cpp，添加命名空间，移动到 `Misaka/src/material/` | 🔴 高 |
| 7.2 | `CommonMaterial` 分离 .h/.cpp，添加命名空间，移动到 `Misaka/src/material/` | 🔴 高 |
| 7.3 | `ColorMaterial` 分离 .h/.cpp，添加命名空间，移动到 `Misaka/src/material/` | 🟡 中 |

### 阶段 8：收尾与测试 (预计 1-2 小时)

| 步骤 | 任务 | 优先级 |
|------|------|--------|
| 8.1 | 创建 `Sandbox/src/main.cpp`，使用 `#include <Misaka/Misaka.h>` | 🔴 高 |
| 8.2 | 编译测试 | 🔴 高 |
| 8.3 | 删除旧的 `src/leave3D/` 目录 | 🟡 中 |
| 8.4 | 更新 README.md | 🟢 低 |

---

## 📊 依赖关系图

```
                    ┌──────────────────┐
                    │   Application    │
                    └────────┬─────────┘
                             │
          ┌──────────────────┼──────────────────┐
          │                  │                  │
          ▼                  ▼                  ▼
    ┌──────────┐      ┌──────────┐      ┌──────────────┐
    │  Scene3D │      │ Renderer │      │ AssetManager │
    └────┬─────┘      └────┬─────┘      └──────┬───────┘
         │                 │                   │
    ┌────┴────┐       ┌────┴────┐        ┌─────┴─────┐
    ▼         ▼       ▼         ▼        ▼           ▼
┌──────────┐ ┌─────────┐ ┌───────┐ ┌───────┐ ┌───────┐ ┌─────────────┐
│ObjectCon-│ │LightBase│ │Geometry│ │ Shader │ │Texture│ │ModelImporter│
│tainer3D  │ └─────────┘ └───────┘ └───────┘ └───────┘ └─────────────┘
└────┬─────┘
     │
┌────┴────┐
▼         ▼
┌──────┐ ┌──────────┐
│ Mesh │ │ Camera3D │
└──┬───┘ └──────────┘
   │
   ▼
┌──────────────┐
│ MaterialBase │
└──────────────┘
```

---

## ⚠️ 注意事项

### 编译顺序
由于存在依赖关系，重构时需注意：
1. 先重构 `Misaka/src/core/` 和 `Misaka/src/resources/IResource.h`（无依赖）
2. 再重构 `Misaka/src/renderer/` 模块（依赖 IResource）
3. 然后重构 `Misaka/src/entity/Object3D.h`（无依赖）
4. 接着重构 `Misaka/src/scene/` 模块（依赖 Object3D）
5. 最后重构 `Misaka/src/material/` 和 `Misaka/src/camera/`

### 保持向后兼容
在重构期间，可以保留旧的头文件作为转发：
```cpp
// src/leave3D/Camera3D.h (deprecated)
#pragma once
#pragma message("Warning: leave3D/Camera3D.h is deprecated, use <Misaka/Misaka.h>")
#include <Misaka/Misaka.h>
```

---

## ✅ 验收标准

- [ ] Library 化项目结构已完成 (`Misaka/`, `Sandbox/`)
- [ ] 所有类已分离 .h 和 .cpp
- [ ] 所有类已添加 `Misaka` 命名空间
- [ ] `Misaka/Misaka.h` 统一入口头文件已创建
- [ ] CMakeLists.txt 配置完成（顶层、Misaka、Sandbox）
- [ ] 项目可正常编译
- [ ] Sandbox 示例应用正常运行（渲染立方体、光照、相机控制）
- [ ] 旧的 `src/leave3D/` 目录已删除

---

## 📖 使用示例

重构完成后，用户代码如下：

**Sandbox/src/main.cpp**
```cpp
#include <Misaka/Misaka.h>
#include <iostream>

using namespace Misaka;

int main() {
    // 初始化窗口和 OpenGL (略)
    
    // 创建场景
    Scene3D* scene = new Scene3D();
    
    // 加载着色器
    auto shader = AssetManager::Ins()->GetOrLoad<Shader>("assets/shader/common", [&](){
        // ... 加载代码
    });
    
    // 加载模型
    auto prefab = ModelImporter::Load("assets/model/cube.obj");
    if (prefab) {
        ObjectContainer3D* cube = SceneBuilder::Instantiate(prefab);
        scene->GetRoot()->AddChild(cube);
    }
    
    // 添加灯光
    DirectionalLight* dirLight = new DirectionalLight();
    dirLight->SetRotation(-45.0f, 45.0f, 0.0f);
    scene->GetRoot()->AddChild(dirLight);
    
    // 创建相机
    Camera3D* camera = new Camera3D(45.0f, 16.0f/9.0f, 0.1f, 100.0f);
    camera->SetPosition(0.0f, 0.0f, 10.0f);
    
    // 创建渲染器
    Renderer* renderer = new Renderer();
    View3D* view3D = new View3D(renderer, scene);
    
    // 渲染循环
    while (!shouldClose) {
        view3D->Render(camera->GetViewMatrix(), camera->GetProjectionMatrix(), camera->GetPosition());
        // ...
    }
    
    // 清理
    delete camera;
    delete view3D;
    delete renderer;
    delete scene;
    
    return 0;
}
```

---

## 📚 参考资料

- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)
- [OpenGL 最佳实践](https://www.khronos.org/opengl/wiki/Common_Mistakes)

---

*文档版本: 1.0*  
*创建日期: 2026-01-02*  
*项目: Misaka3D*
