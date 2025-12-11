#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <string>

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
#include "leave3D/resource/ObjParser.h"
#include "leave3D/material/CommonMaterial.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
int CURRENT_WIDTH = SCR_WIDTH;
int CURRENT_HEIGHT = SCR_HEIGHT;

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

    std::string vCode = ResourceLoader::LoadTextFile("assets/shader/common.vert");
    std::string fCode = ResourceLoader::LoadTextFile("assets/shader/common.frag");
    std::cout << "Vertex Shader Code:\n" << vCode << std::endl;
    std::cout << "Fragment Shader Code:\n" << fCode << std::endl;
    Shader* shader = new Shader(vCode.c_str(), fCode.c_str());

    Geometry* cubeGeo = ObjParser::Parse("assets/model/cube.obj");

    CommonMaterial* redMat = new CommonMaterial(shader);
    redMat->color = glm::vec4(1.0f, 0.2f, 0.2f, 1.0f); // 红色
    redMat->roughness = 0.3f; // 稍微光滑一点
    redMat->metallic = 0.1f;  // 有点金属感
    CommonMaterial* blueMat = new CommonMaterial(shader);
    blueMat->color = glm::vec4(0.2f, 0.2f, 1.0f, 1.0f); // 蓝色
    blueMat->roughness = 0.8f; // 偏粗糙一点
    blueMat->metallic = 0.0f;  // 非金属
    redMat->albedoMap = ResourceLoader::LoadTexture("assets/texture/uv1k00.png");
    blueMat->albedoMap = ResourceLoader::LoadTexture("assets/texture/uv1k01.png");

    // C. 构建场景图
    Scene3D* scene = new Scene3D();

    //添加灯光
    DirectionalLight* dirLight = new DirectionalLight();
    dirLight->SetRotation(-45.0f, 45.0f, 0.0f); // 对角线方向
    scene->GetRoot()->AddChild(dirLight);

    PointLight* pointLight = new PointLight();
    pointLight->SetPosition(0.0f, 0.0f, 0.0f);
    pointLight->color = glm::vec3(1.0f, 0.0f, 0.0f);
    // scene->GetRoot()->AddChild(pointLight);
    
    // 创建一个红色的方块 Mesh
    Mesh* box1 = new Mesh(cubeGeo, redMat);
    scene->GetRoot()->AddChild(box1);

    // 创建一个蓝色的子方块 Mesh
    Mesh* box2 = new Mesh(cubeGeo, blueMat);
    box2->SetPosition(3.0f, 0.0f, 0.0f); // 偏移一点
    box2->SetScale(0.2f, 0.2f, 0.2f);    // 变小一点
    box1->AddChild(box2);                // box2 是 box1 的子节点
    box2->AddChild(pointLight);      // PointLight 是 box2 的子节点

    // D. 准备渲染器和相机
    Renderer* renderer = new Renderer();
    View3D* view3D = new View3D(renderer, scene);
    
    // 初始化相机：FOV 45, 宽高比, 近平面, 远平面
    Camera3D* camera = new Camera3D(45.0f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    camera->SetPosition(0.0f, 0.0f, 10.0f); // 相机往后拉

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // Logic Update
        box1->SetRotation(box1->GetRotation().x + 0.5f, box1->GetRotation().y + 1.0f, 0.0f);
        box2->SetRotation(0.0f, box2->GetRotation().y + 2.0f, 0.0f); // 子物体自转

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
        
        // 执行渲染
        // 注意：Camera3D::GetViewMatrix() 返回值，View3D::Render 接受引用
        // 如果编译报错，可以使用临时变量
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
    delete redMat;
    delete blueMat;
    delete cubeGeo;
    delete shader;

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