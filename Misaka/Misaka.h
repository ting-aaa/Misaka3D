#pragma once

// ============================================
// Misaka3D Engine - 统一入口头文件
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
