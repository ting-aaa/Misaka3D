#include "SceneConfigLoader.h"

#include <fstream>
#include <sstream>

#include "SimpleJson.h"
#include "SceneBuilder.h"
#include "../resources/ModelImporter.h"
#include "../resources/AssetManager.h"
#include "../entity/light/DirectionalLight.h"
#include "../entity/light/PointLight.h"
#include "../entity/Mesh.h"
#include "../entity/Object3D.h"
#include "../material/PBRMaterial.h"
#include "../material/ShaderToyMaterial.h"
#include "../renderer/CubeGeometry.h"
#include "../renderer/SphereGeometry.h"
#include "../renderer/QuadGeometry.h"

namespace Misaka {

namespace {

glm::vec3 ReadVec3(const JsonValue* value, const glm::vec3& fallback) {
    if (!value) return fallback;
    const JsonArray* arr = value->AsArray();
    if (!arr || arr->size() < 3) return fallback;

    return glm::vec3(
        static_cast<float>((*arr)[0].AsNumber(fallback.x)),
        static_cast<float>((*arr)[1].AsNumber(fallback.y)),
        static_cast<float>((*arr)[2].AsNumber(fallback.z))
    );
}

glm::vec4 ReadVec4(const JsonValue* value, const glm::vec4& fallback) {
    if (!value) return fallback;
    const JsonArray* arr = value->AsArray();
    if (!arr || arr->size() < 4) return fallback;

    return glm::vec4(
        static_cast<float>((*arr)[0].AsNumber(fallback.x)),
        static_cast<float>((*arr)[1].AsNumber(fallback.y)),
        static_cast<float>((*arr)[2].AsNumber(fallback.z)),
        static_cast<float>((*arr)[3].AsNumber(fallback.w))
    );
}

const JsonValue* FindValue(const JsonObject* obj, const char* key) {
    if (!obj) return nullptr;
    auto it = obj->find(key);
    if (it == obj->end()) return nullptr;
    return &it->second;
}

std::string LoadTextFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

std::shared_ptr<Shader> FindOrLoadShader(const std::string& shaderBasePath) {
    auto* assets = AssetManager::Ins();
    auto shader = assets->Find<Shader>(shaderBasePath);
    if (shader) return shader;

    std::string vCode = LoadTextFile(shaderBasePath + ".vert");
    std::string fCode = LoadTextFile(shaderBasePath + ".frag");
    if (vCode.empty() || fCode.empty()) {
        return nullptr;
    }

    return assets->GetOrLoad<Shader>(shaderBasePath, [vCode, fCode]() {
        return std::make_shared<Shader>(vCode.c_str(), fCode.c_str());
    });
}

void ApplyTransform(Object3D* object, const JsonValue* transformObj) {
    if (!object || !transformObj) return;

    const JsonObject* obj = transformObj->AsObject();
    if (!obj) return;

    if (auto it = obj->find("position"); it != obj->end()) {
        object->SetPosition(ReadVec3(&it->second, object->GetPosition()));
    }
    if (auto it = obj->find("rotation"); it != obj->end()) {
        object->SetRotation(ReadVec3(&it->second, object->GetRotation()));
    }
    if (auto it = obj->find("scale"); it != obj->end()) {
        object->setScale(ReadVec3(&it->second, object->GetScale()));
    }
}

std::shared_ptr<Geometry> CreatePrimitiveGeometry(const JsonObject* primitiveObj, const std::string& type, int rows, int cols) {
    auto* assets = AssetManager::Ins();

    if (type == "quad" || type == "plane") {
        float size = 2.0f;
        if (const JsonValue* sizeJson = FindValue(primitiveObj, "size")) {
            size = static_cast<float>(sizeJson->AsNumber(2.0f));
        }

        std::string uid = "primitive/quad/s=" + std::to_string(size)
            + "/grid=" + std::to_string(rows) + "x" + std::to_string(cols);

        return assets->GetOrLoad<Geometry>(uid, [=]() {
            return std::make_shared<QuadGeometry>(size);
        });
    }

    if (type == "sphere") {
        const JsonValue* radiusJson = FindValue(primitiveObj, "radius");
        const JsonValue* sectorsJson = FindValue(primitiveObj, "sectors");
        const JsonValue* stacksJson = FindValue(primitiveObj, "stacks");

        float radius = static_cast<float>(radiusJson ? radiusJson->AsNumber(0.5f) : 0.5f);
        int sectors = static_cast<int>(sectorsJson ? sectorsJson->AsNumber(32) : 32);
        int stacks = static_cast<int>(stacksJson ? stacksJson->AsNumber(16) : 16);

        std::string uid = "primitive/sphere/r=" + std::to_string(radius)
            + "/s=" + std::to_string(sectors)
            + "/t=" + std::to_string(stacks)
            + "/grid=" + std::to_string(rows) + "x" + std::to_string(cols);

        return assets->GetOrLoad<Geometry>(uid, [=]() {
            return std::make_shared<SphereGeometry>(radius, sectors, stacks);
        });
    }

    float size = 1.0f;
    if (const JsonValue* sizeJson = FindValue(primitiveObj, "size")) {
        size = static_cast<float>(sizeJson->AsNumber(1.0f));
    }

    std::string uid = "primitive/cube/s=" + std::to_string(size)
        + "/grid=" + std::to_string(rows) + "x" + std::to_string(cols);

    return assets->GetOrLoad<Geometry>(uid, [=]() {
        return std::make_shared<CubeGeometry>(size);
    });
}

std::shared_ptr<MaterialBase> CreateShaderToyMaterial(const JsonObject* materialObj, const std::string& materialUID) {
    auto* assets = AssetManager::Ins();
    std::string shaderPath = "assets/shader/ocean";
    if (materialObj) {
        if (const JsonValue* shaderValue = FindValue(materialObj, "shader")) {
            shaderPath = shaderValue->AsString(shaderPath);
        }
    }

    auto shader = FindOrLoadShader(shaderPath);
    if (!shader) return nullptr;

    auto material = assets->GetOrLoad<ShaderToyMaterial>(materialUID, [shader]() {
        return std::make_shared<ShaderToyMaterial>(shader);
    });
    return material;
}

std::shared_ptr<PBRMaterial> CreatePbrMaterial(
    const JsonObject* materialObj,
    const std::string& materialUID,
    float roughness,
    float metallic
) {
    auto* assets = AssetManager::Ins();
    auto shader = assets->Find<Shader>("assets/shader/pbr");
    if (!shader) {
        shader = assets->Find<Shader>("assets/shader/common");
    }
    if (!shader) return nullptr;

    auto material = assets->GetOrLoad<PBRMaterial>(materialUID, [shader]() {
        return std::make_shared<PBRMaterial>(shader);
    });

    if (!material) return nullptr;

    material->roughness = roughness;
    material->metallic = metallic;
    material->ao = 1.0f;
    material->albedoColor = glm::vec4(1.0f);

    if (materialObj) {
        material->albedoColor = ReadVec4(FindValue(materialObj, "albedo"), material->albedoColor);
        if (const JsonValue* aoValue = FindValue(materialObj, "ao")) {
            material->ao = static_cast<float>(aoValue->AsNumber(material->ao));
        }
    }

    return material;
}

void CreatePrimitiveGrid(Scene3D* scene, const JsonObject* primitiveObj) {
    if (!scene || !primitiveObj) return;

    const std::string type = FindValue(primitiveObj, "type") ? FindValue(primitiveObj, "type")->AsString("sphere") : "sphere";
    const std::string name = FindValue(primitiveObj, "name") ? FindValue(primitiveObj, "name")->AsString("PrimitiveGrid") : "PrimitiveGrid";

    const JsonObject* gridObj = nullptr;
    if (const JsonValue* gridValue = FindValue(primitiveObj, "grid")) {
        gridObj = gridValue->AsObject();
    }
    if (!gridObj) return;

    int rows = static_cast<int>(FindValue(gridObj, "rows") ? FindValue(gridObj, "rows")->AsNumber(10) : 10);
    int cols = static_cast<int>(FindValue(gridObj, "cols") ? FindValue(gridObj, "cols")->AsNumber(10) : 10);
    float spacing = static_cast<float>(FindValue(gridObj, "spacing") ? FindValue(gridObj, "spacing")->AsNumber(1.5f) : 1.5f);
    std::string plane = FindValue(gridObj, "plane") ? FindValue(gridObj, "plane")->AsString("xz") : "xz";

    rows = rows < 1 ? 1 : rows;
    cols = cols < 1 ? 1 : cols;

    auto geometry = CreatePrimitiveGeometry(primitiveObj, type, rows, cols);
    if (!geometry) return;

    const JsonObject* materialObj = nullptr;
    if (const JsonValue* matValue = FindValue(primitiveObj, "material")) {
        materialObj = matValue->AsObject();
    }

    glm::vec2 roughRange(0.05f, 1.0f);
    glm::vec2 metalRange(0.0f, 1.0f);
    if (materialObj) {
        if (const JsonValue* roughRangeValue = FindValue(materialObj, "roughnessRange")) {
            if (const JsonArray* arr = roughRangeValue->AsArray()) {
                if (arr->size() >= 2) {
                    roughRange.x = static_cast<float>((*arr)[0].AsNumber(roughRange.x));
                    roughRange.y = static_cast<float>((*arr)[1].AsNumber(roughRange.y));
                }
            }
        }
        if (const JsonValue* metalRangeValue = FindValue(materialObj, "metallicRange")) {
            if (const JsonArray* arr = metalRangeValue->AsArray()) {
                if (arr->size() >= 2) {
                    metalRange.x = static_cast<float>((*arr)[0].AsNumber(metalRange.x));
                    metalRange.y = static_cast<float>((*arr)[1].AsNumber(metalRange.y));
                }
            }
        }
    }

    auto* parent = new ObjectContainer3D();
    parent->SetName(name);
    scene->GetRoot()->AddChild(parent);

    float xStart = -(static_cast<float>(cols - 1) * spacing) * 0.5f;
    float zStart = -(static_cast<float>(rows - 1) * spacing) * 0.5f;

    for (int r = 0; r < rows; ++r) {
        float roughT = rows == 1 ? 0.0f : static_cast<float>(r) / static_cast<float>(rows - 1);
        float roughness = roughRange.x + (roughRange.y - roughRange.x) * roughT;

        for (int c = 0; c < cols; ++c) {
            float metalT = cols == 1 ? 0.0f : static_cast<float>(c) / static_cast<float>(cols - 1);
            float metallic = metalRange.x + (metalRange.y - metalRange.x) * metalT;

            std::string matUID = "primitive/pbr/" + name + "/r=" + std::to_string(r) + "/c=" + std::to_string(c);
            auto material = CreatePbrMaterial(materialObj, matUID, roughness, metallic);
            if (!material) continue;

            auto* mesh = new Mesh(geometry, material);
            mesh->SetName(type + "_" + std::to_string(r) + "_" + std::to_string(c));
            const float p0 = xStart + c * spacing;
            const float p1 = zStart + r * spacing;
            if (plane == "xy") {
                mesh->SetPosition(p0, p1, 0.0f);
            } else if (plane == "yz") {
                mesh->SetPosition(0.0f, p1, p0);
            } else {
                mesh->SetPosition(p0, 0.0f, p1);
            }
            parent->AddChild(mesh);
        }
    }

    if (const JsonValue* transform = FindValue(primitiveObj, "transform")) {
        ApplyTransform(parent, transform);
    }
}

void CreateSinglePrimitive(Scene3D* scene, const JsonObject* primitiveObj) {
    if (!scene || !primitiveObj) return;

    const std::string type = FindValue(primitiveObj, "type") ? FindValue(primitiveObj, "type")->AsString("sphere") : "sphere";
    auto geometry = CreatePrimitiveGeometry(primitiveObj, type, 1, 1);
    if (!geometry) return;

    const JsonObject* materialObj = nullptr;
    if (const JsonValue* matValue = FindValue(primitiveObj, "material")) {
        materialObj = matValue->AsObject();
    }

    float roughness = materialObj && FindValue(materialObj, "roughness")
        ? static_cast<float>(FindValue(materialObj, "roughness")->AsNumber(0.5f))
        : 0.5f;
    float metallic = materialObj && FindValue(materialObj, "metallic")
        ? static_cast<float>(FindValue(materialObj, "metallic")->AsNumber(0.0f))
        : 0.0f;

    const std::string name = FindValue(primitiveObj, "name") ? FindValue(primitiveObj, "name")->AsString(type) : type;
    std::shared_ptr<MaterialBase> material;
    const std::string matType = materialObj && FindValue(materialObj, "type")
        ? FindValue(materialObj, "type")->AsString("pbr")
        : "pbr";

    if (matType == "shadertoy") {
        material = CreateShaderToyMaterial(materialObj, "primitive/shadertoy/single/" + name);
    } else {
        material = CreatePbrMaterial(materialObj, "primitive/pbr/single/" + name, roughness, metallic);
    }
    if (!material) return;

    auto* mesh = new Mesh(geometry, material);
    mesh->SetName(name);
    if (const JsonValue* transform = FindValue(primitiveObj, "transform")) {
        ApplyTransform(mesh, transform);
    }
    scene->GetRoot()->AddChild(mesh);
}

} // namespace

SceneLoadResult SceneConfigLoader::LoadFromFile(Scene3D* scene, const std::string& configPath) {
    SceneLoadResult result;
    if (!scene) {
        result.error = "Scene is null";
        return result;
    }

    std::ifstream input(configPath);
    if (!input.is_open()) {
        result.error = "Failed to open scene config: " + configPath;
        return result;
    }

    std::stringstream buffer;
    buffer << input.rdbuf();

    JsonValue root;
    if (!SimpleJsonParser::Parse(buffer.str(), root, &result.error)) {
        result.error = "Scene config parse failed: " + result.error;
        return result;
    }

    const JsonObject* rootObj = root.AsObject();
    if (!rootObj) {
        result.error = "Scene config root must be a JSON object";
        return result;
    }

    // Models
    if (auto it = rootObj->find("models"); it != rootObj->end()) {
        if (const JsonArray* models = it->second.AsArray()) {
            for (const auto& modelEntry : *models) {
                const JsonObject* modelObj = modelEntry.AsObject();
                if (!modelObj) continue;

                auto pathIt = modelObj->find("path");
                if (pathIt == modelObj->end()) continue;
                const std::string modelPath = pathIt->second.AsString();
                if (modelPath.empty()) continue;

                auto prefab = ModelImporter::Load(modelPath);
                if (!prefab) continue;

                ObjectContainer3D* instance = SceneBuilder::Instantiate(prefab);
                if (!instance) continue;

                auto transformIt = modelObj->find("transform");
                if (transformIt != modelObj->end()) {
                    ApplyTransform(instance, &transformIt->second);
                }

                auto nameIt = modelObj->find("name");
                if (nameIt != modelObj->end()) {
                    instance->SetName(nameIt->second.AsString());
                }

                scene->GetRoot()->AddChild(instance);
            }
        }
    }

    // Primitives
    if (auto it = rootObj->find("primitives"); it != rootObj->end()) {
        if (const JsonArray* primitives = it->second.AsArray()) {
            for (const auto& primitiveValue : *primitives) {
                const JsonObject* primitiveObj = primitiveValue.AsObject();
                if (!primitiveObj) continue;

                if (FindValue(primitiveObj, "grid")) {
                    CreatePrimitiveGrid(scene, primitiveObj);
                } else {
                    CreateSinglePrimitive(scene, primitiveObj);
                }
            }
        }
    }

    // Lights
    if (auto lightIt = rootObj->find("lights"); lightIt != rootObj->end()) {
        const JsonObject* lightsObj = lightIt->second.AsObject();
        if (lightsObj) {
            if (auto dirIt = lightsObj->find("directional"); dirIt != lightsObj->end()) {
                if (const JsonArray* dirLights = dirIt->second.AsArray()) {
                    for (const auto& item : *dirLights) {
                        const JsonObject* obj = item.AsObject();
                        if (!obj) continue;

                        auto* light = new DirectionalLight();
                        light->color = ReadVec3(FindValue(obj, "color"), light->color);
                        if (const JsonValue* intensity = FindValue(obj, "intensity")) {
                            light->intensity = static_cast<float>(intensity->AsNumber(light->intensity));
                        }
                        light->SetRotation(ReadVec3(FindValue(obj, "rotation"), glm::vec3(-45.0f, 45.0f, 0.0f)));
                        scene->GetRoot()->AddChild(light);
                    }
                }
            }

            if (auto pointIt = lightsObj->find("point"); pointIt != lightsObj->end()) {
                if (const JsonArray* pointLights = pointIt->second.AsArray()) {
                    for (const auto& item : *pointLights) {
                        const JsonObject* obj = item.AsObject();
                        if (!obj) continue;

                        auto* light = new PointLight();
                        light->color = ReadVec3(FindValue(obj, "color"), light->color);
                        if (const JsonValue* intensity = FindValue(obj, "intensity")) {
                            light->intensity = static_cast<float>(intensity->AsNumber(light->intensity));
                        }
                        light->SetPosition(ReadVec3(FindValue(obj, "position"), light->GetPosition()));
                        if (const JsonValue* range = FindValue(obj, "range")) {
                            light->SetRange(static_cast<float>(range->AsNumber(light->radius)));
                        }
                        scene->GetRoot()->AddChild(light);
                    }
                }
            }
        }
    }

    // Camera
    if (auto cameraIt = rootObj->find("camera"); cameraIt != rootObj->end()) {
        const JsonObject* cameraObj = cameraIt->second.AsObject();
        if (cameraObj) {
            const JsonValue* fovJson = FindValue(cameraObj, "fov");
            const JsonValue* aspectJson = FindValue(cameraObj, "aspect");
            const JsonValue* nearJson = FindValue(cameraObj, "near");
            const JsonValue* farJson = FindValue(cameraObj, "far");

            const float fov = static_cast<float>(fovJson ? fovJson->AsNumber(45.0f) : 45.0f);
            const float aspect = static_cast<float>(aspectJson ? aspectJson->AsNumber(16.0 / 9.0) : 16.0f / 9.0f);
            const float nearPlane = static_cast<float>(nearJson ? nearJson->AsNumber(0.1f) : 0.1f);
            const float farPlane = static_cast<float>(farJson ? farJson->AsNumber(100.0f) : 100.0f);

            result.camera = new Camera3D(fov, aspect, nearPlane, farPlane);
            result.camera->SetPosition(ReadVec3(FindValue(cameraObj, "position"), glm::vec3(0.0f, 0.0f, 10.0f)));
            result.camera->SetRotation(ReadVec3(FindValue(cameraObj, "rotation"), glm::vec3(0.0f)));
        }
    }

    result.success = true;
    return result;
}

} // namespace Misaka
