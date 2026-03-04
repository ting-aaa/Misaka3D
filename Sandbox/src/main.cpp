#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <cstdio>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <Misaka/Misaka.h>

using namespace Misaka;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
int CURRENT_WIDTH = SCR_WIDTH;
int CURRENT_HEIGHT = SCR_HEIGHT;

namespace {

void DrawObjectNode(ObjectContainer3D* object, ObjectContainer3D*& selected) {
    if (!object) return;

    std::string label = object->GetName().empty() ? "(unnamed)" : object->GetName();
    bool opened = ImGui::TreeNodeEx(
        object,
        (selected == object ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow,
        "%s",
        label.c_str()
    );

    if (ImGui::IsItemClicked()) {
        selected = object;
    }

    if (opened) {
        for (auto* child : object->GetChildren()) {
            DrawObjectNode(child, selected);
        }
        ImGui::TreePop();
    }
}

void DrawObjectInspector(ObjectContainer3D* object) {
    if (!object) {
        ImGui::Text("No object selected.");
        return;
    }

    char nameBuf[128] = {};
    const std::string& currentName = object->GetName();
    std::snprintf(nameBuf, sizeof(nameBuf), "%s", currentName.c_str());
    if (ImGui::InputText("Name", nameBuf, sizeof(nameBuf))) {
        object->SetName(nameBuf);
    }

    glm::vec3 pos = object->GetPosition();
    glm::vec3 rot = object->GetRotation();
    glm::vec3 scl = object->GetScale();

    if (ImGui::DragFloat3("Position", &pos.x, 0.05f)) object->SetPosition(pos);
    if (ImGui::DragFloat3("Rotation", &rot.x, 0.5f)) object->SetRotation(rot);
    if (ImGui::DragFloat3("Scale", &scl.x, 0.05f, 0.001f, 100.0f)) object->setScale(scl);

    if (auto* light = dynamic_cast<LightBase*>(object)) {
        ImGui::SeparatorText("Light");
        ImGui::Checkbox("Enabled", &light->enabled);
        ImGui::ColorEdit3("Color", &light->color.x);
        ImGui::DragFloat("Intensity", &light->intensity, 0.05f, 0.0f, 100.0f);

        if (auto* point = dynamic_cast<PointLight*>(light)) {
            if (ImGui::DragFloat("Range", &point->radius, 0.1f, 0.1f, 500.0f)) {
                point->SetRange(point->radius);
            }
        }
    }

    if (auto* mesh = dynamic_cast<Mesh*>(object)) {
        ImGui::SeparatorText("Mesh");
        auto mat = mesh->GetMaterial();
        if (mat) {
            ImGui::Text("Material UID: %s", mat->GetUID().c_str());
            if (auto pbr = std::dynamic_pointer_cast<PBRMaterial>(mat)) {
                ImGui::DragFloat("Roughness", &pbr->roughness, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Metallic", &pbr->metallic, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("AO", &pbr->ao, 0.01f, 0.0f, 1.0f);
                ImGui::ColorEdit4("Albedo", &pbr->albedoColor.x);
            }
        }
    }
}

void DrawModelPrefabEditor(const std::shared_ptr<ModelPrefab>& prefab) {
    if (!prefab) {
        ImGui::Text("No prefab loaded.");
        return;
    }

    ImGui::Text("Mesh Count: %zu", prefab->GetMeshCount());
    for (size_t i = 0; i < prefab->GetMeshCount(); ++i) {
        const auto* entry = prefab->GetMeshEntry(i);
        if (!entry) continue;

        if (ImGui::TreeNode(reinterpret_cast<void*>(i + 1), "%s", entry->name.c_str())) {
            ImGui::Text("Geometry UID: %s", entry->geometry ? entry->geometry->GetUID().c_str() : "<null>");
            ImGui::Text("Material UID: %s", entry->material ? entry->material->GetUID().c_str() : "<null>");

            if (entry->material) {
                auto pbr = std::dynamic_pointer_cast<PBRMaterial>(entry->material);
                if (pbr) {
                    ImGui::DragFloat(("Roughness##" + std::to_string(i)).c_str(), &pbr->roughness, 0.01f, 0.0f, 1.0f);
                    ImGui::DragFloat(("Metallic##" + std::to_string(i)).c_str(), &pbr->metallic, 0.01f, 0.0f, 1.0f);
                }
            }

            ImGui::TreePop();
        }
    }
}

void DrawResourceList() {
    auto* assets = AssetManager::Ins();

    if (ImGui::CollapsingHeader("Textures", ImGuiTreeNodeFlags_DefaultOpen)) {
        auto textures = assets->GetAllAssets<Texture>();
        for (const auto& tex : textures) {
            ImGui::BulletText("%s (%dx%d)", tex->GetUID().c_str(), tex->width, tex->height);
        }
    }

    if (ImGui::CollapsingHeader("Materials", ImGuiTreeNodeFlags_DefaultOpen)) {
        auto materials = assets->GetAllAssets<MaterialBase>();
        for (const auto& mat : materials) {
            ImGui::BulletText("%s", mat->GetUID().c_str());
        }
    }

    if (ImGui::CollapsingHeader("Shaders", ImGuiTreeNodeFlags_DefaultOpen)) {
        auto shaders = assets->GetAllAssets<Shader>();
        for (const auto& shader : shaders) {
            ImGui::BulletText("%s", shader->GetUID().c_str());
        }
    }

    if (ImGui::CollapsingHeader("Model Prefabs", ImGuiTreeNodeFlags_DefaultOpen)) {
        auto prefabs = assets->GetAllAssets<ModelPrefab>();
        for (const auto& prefab : prefabs) {
            ImGui::BulletText("%s (meshes:%zu)", prefab->GetUID().c_str(), prefab->GetMeshCount());
        }
    }
}

} // namespace

std::string LoadTextFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << path << std::endl;
        return "";
    }
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}

int main() {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Misaka3D Engine", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // ImGui Init
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);

    // ==========================================
    // Misaka3D 引擎初始化
    // ==========================================

    std::shared_ptr<Shader> shader = AssetManager::Ins()->GetOrLoad<Shader>("assets/shader/common", [&]() {
        std::string vCode = LoadTextFile("assets/shader/common.vert");
        std::string fCode = LoadTextFile("assets/shader/common.frag");
        return std::make_shared<Shader>(vCode.c_str(), fCode.c_str());
    });

    std::shared_ptr<Shader> pbrShader = AssetManager::Ins()->GetOrLoad<Shader>("assets/shader/pbr", [&]() {
        std::string vCode = LoadTextFile("assets/shader/pbr.vert");
        std::string fCode = LoadTextFile("assets/shader/pbr.frag");
        return std::make_shared<Shader>(vCode.c_str(), fCode.c_str());
    });

    Scene3D* scene = new Scene3D();
    std::shared_ptr<ModelPrefab> prefab = nullptr;
    ObjectContainer3D* animatedObject = nullptr;

    auto sceneLoad = SceneConfigLoader::LoadFromFile(scene, "assets/scene/ocean_shadertoy.scene.json");
    if (!sceneLoad.success) {
        std::cout << "Scene config load failed: " << sceneLoad.error << std::endl;
    }

    if (!sceneLoad.success || scene->GetRoot()->GetChildren().empty()) {
        DirectionalLight* dirLight = new DirectionalLight();
        dirLight->SetRotation(-45.0f, 45.0f, 0.0f);
        scene->GetRoot()->AddChild(dirLight);

        prefab = ModelImporter::Load("assets/model/cube.obj");
        if (prefab) {
            animatedObject = SceneBuilder::Instantiate(prefab);
            scene->GetRoot()->AddChild(animatedObject);
        }
    }

    if (!prefab) {
        prefab = ModelImporter::Load("assets/model/cube.obj");
    }

    Renderer* renderer = new Renderer();
    View3D* view3D = new View3D(renderer, scene); 
    
    Camera3D* camera = sceneLoad.camera;
    if (!camera) {
        camera = new Camera3D(45.0f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        camera->SetPosition(0.0f, 0.0f, 10.0f);
    }

    ObjectContainer3D* selectedObject = scene->GetRoot();

    // render loop
    while (!glfwWindowShouldClose(window)) {
        if (animatedObject) {
            glm::vec3 rot = animatedObject->GetRotation();
            animatedObject->SetRotation(rot.x + 0.5f, rot.y, rot.z);
        }

        // Render Clear
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Input
        processInput(window);

        // ==========================================
        // Misaka3D 渲染
        // ==========================================
        camera->SetAspect((float)CURRENT_WIDTH / (float)CURRENT_HEIGHT);
        
        glm::mat4 view = camera->GetViewMatrix();
        glm::mat4 proj = camera->GetProjectionMatrix();
        view3D->Render(view, proj, camera->GetPosition());

        // ImGui Render
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (ImGui::Begin("Scene Editor")) {
            if (ImGui::CollapsingHeader("Scene Hierarchy", ImGuiTreeNodeFlags_DefaultOpen)) {
                DrawObjectNode(scene->GetRoot(), selectedObject);
            }
            if (ImGui::CollapsingHeader("Object Inspector", ImGuiTreeNodeFlags_DefaultOpen)) {
                DrawObjectInspector(selectedObject);
            }
        }
        ImGui::End();

        if (ImGui::Begin("ModelPrefab Editor")) {
            DrawModelPrefabEditor(prefab);
        }
        ImGui::End();

        if (ImGui::Begin("Resource List")) {
            DrawResourceList();
        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    delete camera;
    delete view3D;
    delete renderer;
    delete scene;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    CURRENT_WIDTH = width;
    CURRENT_HEIGHT = height;
    glViewport(0, 0, width, height);
}
