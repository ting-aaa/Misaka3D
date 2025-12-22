#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"

#include "leave3D/Scene3D.h"
#include "leave3D/View3D.h"
#include "leave3D/Camera3D.h" // 确保你创建了这个文件
#include "leave3D/Mesh.h"
#include "leave3D/Geometry.h"
#include "leave3D/material/MaterialBase.h"
#include "leave3D/material/Shader.h"
#include "leave3D/material/CommonMaterial.h"
#include "leave3D/AssetManager.h"
#include "leave3D/ModelImporter.h"
#include "leave3D/SceneBuilder.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
int CURRENT_WIDTH = SCR_WIDTH;
int CURRENT_HEIGHT = SCR_HEIGHT;

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

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL + Leave3D", NULL, NULL);
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
    // 4. Leave3D 引擎初始化
    // ==========================================

    std::shared_ptr<Shader> shader = AssetManager::Ins()->GetOrLoad<Shader>("assets/shader/common", [&](){
        std::string vCode = LoadTextFile("assets/shader/common.vert");
        std::string fCode = LoadTextFile("assets/shader/common.frag");
        // std::cout << "Vertex Shader Code:\n" << vCode << std::endl;
        // std::cout << "Fragment Shader Code:\n" << fCode << std::endl;
        std::shared_ptr<Shader> ret = std::make_shared<Shader>(vCode.c_str(), fCode.c_str());
        return ret;
    });

    Scene3D* scene = new Scene3D();

    //添加灯光
    DirectionalLight* dirLight = new DirectionalLight();
    dirLight->SetRotation(-45.0f, 45.0f, 0.0f); // 对角线方向
    scene->GetRoot()->AddChild(dirLight);

    ObjectContainer3D* cube = nullptr;
    auto prefab = ModelImporter::Load("assets/model/cube.obj");
    if(prefab) {
        cube = SceneBuilder::Instantiate(prefab);
        scene->GetRoot()->AddChild(cube);
    }

    Renderer* renderer = new Renderer();
    View3D* view3D = new View3D(renderer, scene);
    
    // 初始化相机：FOV 45, 宽高比, 近平面, 远平面
    Camera3D* camera = new Camera3D(45.0f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    camera->SetPosition(0.0f, 0.0f, 10.0f); // 相机往后拉

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        if(cube) {
            cube->SetRotation(cube->GetRotation().x + 0.5f, 0.0f, 0.0f);
        }

        // Render Clear
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Input
        processInput(window);

        // ==========================================
        // 5. Leave3D 渲染
        // ==========================================
        // 更新相机宽高比（以防窗口缩放）
        camera->SetAspect((float)CURRENT_WIDTH / (float)CURRENT_HEIGHT);
        
        glm::mat4 view = camera->GetViewMatrix();
        glm::mat4 proj = camera->GetProjectionMatrix();
        view3D->Render(view, proj, camera->GetPosition());

        // ImGui Render
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        // ... 你的 ImGui 代码 ...
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup (简单清理，实际项目可以用智能指针)
    delete camera;
    delete view3D;
    delete renderer;
    delete scene; // 会自动删除 box1, box2
    // delete redMat;
    // delete blueMat;
    // delete cubeGeo;
    // delete shader;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}

//============================================
//============================================

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    CURRENT_WIDTH = width;
    CURRENT_HEIGHT = height;
    glViewport(0, 0, width, height);
}