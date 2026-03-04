# Misaka3D 修改清单与 3W 文档（What / How / Why）

> 目标：帮助你系统学习本轮实现过的所有核心改动，包括“改了什么、怎么做、为什么这样做”。

---

## 1) 总览（按能力维度）

本轮实现覆盖了以下能力：

1. 模型导入增强（含内嵌纹理）
2. `ModelPrefab` 可编辑接口扩展
3. 材质 Uniform 系统升级（含 ShaderToy 兼容字段）
4. 渲染上下文与灯光/相机数据管线增强
5. PBR 材质类型与独立 PBR Shader
6. 可程序化几何体（Cube / Sphere / Quad）
7. JSON 场景配置扩展（模型、灯光、primitive、grid、材质类型）
8. ShaderToy 风格海面场景复刻（Ocean）
9. Sandbox 中 ImGui 编辑器面板（场景树/对象检查器/Prefab/资源列表）

---

## 2) 修改清单（文件级）

## 2.1 新增文件

### Material
- `Misaka/src/material/MaterialContext.h`
- `Misaka/src/material/PBRMaterial.h`
- `Misaka/src/material/PBRMaterial.cpp`
- `Misaka/src/material/ShaderToyMaterial.h`
- `Misaka/src/material/ShaderToyMaterial.cpp`

### Renderer Geometry
- `Misaka/src/renderer/CubeGeometry.h`
- `Misaka/src/renderer/CubeGeometry.cpp`
- `Misaka/src/renderer/SphereGeometry.h`
- `Misaka/src/renderer/SphereGeometry.cpp`
- `Misaka/src/renderer/QuadGeometry.h`
- `Misaka/src/renderer/QuadGeometry.cpp`

### Scene System
- `Misaka/src/scene/SimpleJson.h`
- `Misaka/src/scene/SimpleJson.cpp`
- `Misaka/src/scene/SceneConfigLoader.h`
- `Misaka/src/scene/SceneConfigLoader.cpp`

### Shader Assets
- `assets/shader/pbr.vert`
- `assets/shader/pbr.frag`
- `assets/shader/ocean.vert`
- `assets/shader/ocean.frag`

### Scene Assets
- `assets/scene/default.scene.json`
- `assets/scene/pbr_metal_roughness.scene.json`
- `assets/scene/ocean_shadertoy.scene.json`

### Docs
- `docs/IMPLEMENTATION_3W_AND_CHANGELOG.md`（本文）

## 2.2 修改文件

- `Misaka/CMakeLists.txt`
- `Misaka/Misaka.h`
- `Misaka/src/camera/Camera3D.h`
- `Misaka/src/camera/Camera3D.cpp`
- `Misaka/src/entity/ModelPrefab.h`
- `Misaka/src/material/MaterialBase.h`
- `Misaka/src/material/MaterialBase.cpp`
- `Misaka/src/material/CommonMaterial.h`
- `Misaka/src/material/CommonMaterial.cpp`
- `Misaka/src/material/ColorMaterial.h`
- `Misaka/src/material/ColorMaterial.cpp`
- `Misaka/src/renderer/Shader.h`
- `Misaka/src/renderer/Shader.cpp`
- `Misaka/src/renderer/Renderer.h`
- `Misaka/src/renderer/Renderer.cpp`
- `Misaka/src/resources/TextureLoader.h`
- `Misaka/src/resources/TextureLoader.cpp`
- `Misaka/src/resources/ModelImporter.cpp`
- `Sandbox/src/main.cpp`

---

## 3) 3W 详细说明（What / How / Why）

## 3.1 模型导入增强：支持内嵌纹理

### What
- `ModelImporter` 支持 Assimp 的内嵌纹理引用（如 `*0`）。
- 支持两类内嵌纹理数据：
  - 压缩二进制（通过 `stbi_load_from_memory`）
  - 原始 RGBA texel 数组（`aiTexel`）

### How
- 在 `ModelImporter.cpp` 中检测纹理路径是否是内嵌引用。
- 通过 `scene->GetEmbeddedTexture(...)` 取 `aiTexture`。
- 调用 `TextureLoader::LoadFromMemory(...)` 或 `TextureLoader::LoadRawRGBA(...)` 创建 OpenGL 纹理。
- 用 `AssetManager` 做 UID 缓存，避免重复上传。

### Why
- 许多 glTF/FBX/Assimp 流程会打包贴图到模型内部。
- 不支持内嵌纹理会导致材质丢贴图、表现错误。

---

## 3.2 `ModelPrefab` 接口扩展

### What
- 新增了可读写 mesh/material 的常用接口：
  - `GetMeshCount()`
  - `GetMeshEntry(index)`
  - `GetMaterial(meshIndex)`
  - `SetMaterial(meshIndex, material)`
  - `SetMaterialByMeshName(name, material)`

### How
- 在 `ModelPrefab.h` 内直接补充轻量内联函数。

### Why
- 便于运行时做 prefab 级材质替换和编辑器联动。
- 降低外部代码直接操作 `meshes` 的耦合。

---

## 3.3 材质 Uniform 系统升级（含 ShaderToy 兼容）

### What
- `MaterialBase::uploadUniforms()` 升级为带上下文参数：
  - `uploadUniforms(const MaterialContext& context)`
- 增加统一上下文字段：模型/视图/投影、相机、分辨率、时间、帧号、鼠标、通道纹理。
- 增加自定义 Uniform 存储容器（`std::variant`）。
- 支持 ShaderToy 关键 uniform：
  - `iTime`, `iTimeDelta`, `iFrame`, `iResolution`, `iMouse`, `iChannel0..3`

### How
- 新增 `MaterialContext` 结构体。
- `MaterialBase` 新增 `UploadCommonContextUniforms()` 与 `UploadCustomUniforms()`。
- `CommonMaterial/ColorMaterial/PBRMaterial` 统一改用新签名。
- `Shader` 新增 `SetVec2` 以支持 `vec2` uniform。

### Why
- 统一材质传参模型，避免每个材质重复写基础 uniform 上传逻辑。
- 为后续 shader 类型扩展（PBR、ShaderToy）提供共用基础。

---

## 3.4 渲染器上下文管线增强

### What
- `Renderer` 维护启动时间、上一帧时间、帧计数。
- 每次绘制构建 `MaterialContext`，再交给材质上传。

### How
- `Renderer` 增加成员：`_startTime`, `_lastFrameTime`, `_frameCounter`。
- 使用 `std::chrono` 计算 `time/deltaTime`。
- 读取 `GL_VIEWPORT` 得到 `resolution`。

### Why
- 把时间与分辨率统一传给所有材质，支持动画与后处理风格 shader。

---

## 3.5 相机系统增强

### What
- `Camera3D` 增加接口：
  - `SetFov`, `SetNearFar`
  - `GetFov`, `GetAspect`, `GetNearPlane`, `GetFarPlane`
  - `GetViewProjectionMatrix`

### How
- 在 `Camera3D.h/.cpp` 增加 getter/setter 并复用 `UpdateProjection()`。

### Why
- 为场景配置与编辑器暴露更完整相机控制入口。

---

## 3.6 PBR 材质 + 独立 PBR Shader

### What
- 新增 `PBRMaterial` 类型（albedo/normal/metallic/roughness/ao/emissive）。
- 新增独立 `pbr.vert/pbr.frag`，不再与 `common` 混用。
- `ModelImporter` 与 `SceneConfigLoader` 的 PBR 材质创建逻辑优先绑定 `assets/shader/pbr`，找不到再回退 `common`。

### How
- `PBRMaterial::uploadUniforms` 上传全部贴图与 fallback 标量参数。
- `pbr.frag` 使用 Cook-Torrance（GGX + Smith + Schlick）模型，并支持 dir/point light、ao、emissive、tonemap/gamma。

### Why
- “材质类型”与“shader 实现”必须一一对应，便于维护与扩展。

---

## 3.7 几何体系统扩展（继承 `Geometry`）

### What
- 新增三种程序化几何：
  - `CubeGeometry`
  - `SphereGeometry`
  - `QuadGeometry`

### How
- 生成 position/normal/uv 后，复用 `Geometry::ComputeTangents(...)` 计算切线，上传 GPU。

### Why
- 方便测试材质与光照，不依赖外部模型。
- `Quad` 是 ShaderToy 场景的基础载体。

---

## 3.8 JSON 场景系统扩展

### What
- 新增轻量 JSON 解析器 `SimpleJson`。
- 新增场景加载器 `SceneConfigLoader`。
- 支持内容：
  - `models`
  - `lights.directional` / `lights.point`
  - `camera`
  - `primitives`（`sphere/cube/quad/plane`）
  - `grid` 批量生成（rows/cols/spacing/plane）
  - 材质类型选择（`pbr` / `shadertoy`）

### How
- `SceneConfigLoader` 根据 JSON 创建节点并挂到 `Scene3D::root`。
- `grid.plane` 支持 `xy/xz/yz`，用于控制网格朝向。

### Why
- 把“场景描述”从 C++ 代码迁移到数据配置，便于调参与编辑器化。

---

## 3.9 PBR 展示场景（10x10 Sphere）

### What
- 新增 `pbr_metal_roughness.scene.json`。
- 10x10 sphere 网格展示：
  - 一维变化粗糙度
  - 一维变化金属度

### How
- 通过 `grid + roughnessRange + metallicRange` 自动实例化。

### Why
- 这是验证 PBR 参数空间最直观的基准场景。

---

## 3.10 ShaderToy 海面场景（Ocean）

### What
- 新增 `ocean_shadertoy.scene.json`，使用 `quad + shadertoy material`。
- 新增 `ocean.vert/ocean.frag`，将你提供的 ShaderToy `mainImage` 逻辑接入 GLSL 330 管线。

### How
- 在 `ocean.frag` 中补充 `main()`：`mainImage(FragColor, gl_FragCoord.xy)`。
- `ShaderToyMaterial` 上传 `iDate`、`iFrameRate`、`iChannelTime/Resolution` 等参数。

### Why
- 验证 ShaderToy 风格程序化特效在引擎内可复现。

---

## 3.11 ImGui 编辑器能力

### What
- `Sandbox/main.cpp` 中新增面板：
  - Scene Hierarchy
  - Object Inspector
  - ModelPrefab Editor
  - Resource List

### How
- 遍历 `ObjectContainer3D` 树并做类型分派（`LightBase` / `Mesh` / `PBRMaterial`）。

### Why
- 快速可视化调试资源与参数，降低迭代成本。

---

## 4) 运行与验证说明

## 4.1 常见现象

1. **修改 scene.json 后运行效果未更新**
   - 原因：运行时读的是 `build/Sandbox/assets/...`（复制产物），不是源码目录。
   - 解决：重新构建或手动复制 `assets` 到 `build/Sandbox/assets`。

2. **链接失败：`Sandbox.exe` Permission denied**
   - 原因：可执行文件正在运行被占用。
   - 解决：先关闭进程再重建。

## 4.2 建议实践
- 场景与 shader 调整后，统一执行一次构建（触发资源复制）。
- 后续可增加“启动前自动资源同步脚本”。

---

## 5) 你可以按这个学习顺序阅读代码

1. `MaterialBase` + `MaterialContext`（统一材质参数体系）
2. `Renderer`（上下文如何进入 shader）
3. `PBRMaterial` + `assets/shader/pbr.frag`（PBR闭环）
4. `SceneConfigLoader`（数据驱动场景）
5. `ShaderToyMaterial` + `assets/shader/ocean.frag`（特效扩展）
6. `Sandbox/main.cpp`（编辑器与运行入口）

---

## 6) 后续可继续改进（建议）

1. 增加 Shader 编译错误日志（当前 `Shader.cpp` 未输出 compile/link log）。
2. 将 `SimpleJson` 替换为成熟 JSON 库（错误信息和兼容性更完整）。
3. 把资源同步改为 CMake/启动脚本自动化，避免“改了场景但运行不生效”。
4. 为 SceneConfig 增加 schema/版本号，避免字段漂移。

---

> 如果你希望，我可以继续在 `docs` 里再生成一份“按提交粒度的学习版文档”（每个功能一个小节，附最小可运行代码路径）。
