#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>

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

    Scene3D* scene = new Scene3D();

    // 添加灯光
    DirectionalLight* dirLight = new DirectionalLight();
    dirLight->SetRotation(-45.0f, 45.0f, 0.0f);
    scene->GetRoot()->AddChild(dirLight);

    ObjectContainer3D* cube = nullptr;
    auto prefab = ModelImporter::Load("assets/model/cube.obj");
    if (prefab) {
        cube = SceneBuilder::Instantiate(prefab);
        scene->GetRoot()->AddChild(cube);
    }

    Renderer* renderer = new Renderer();
    View3D* view3D = new View3D(renderer, scene); 
    
    Camera3D* camera = new Camera3D(45.0f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    camera->SetPosition(0.0f, 0.0f, 10.0f);

    // render loop
    while (!glfwWindowShouldClose(window)) {
        if (cube) {
            cube->SetRotation(cube->GetRotation().x + 0.5f, 0.0f, 0.0f);
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
        // ... ImGui 代码 ...
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
