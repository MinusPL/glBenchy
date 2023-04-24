#include <cstdio>

#ifndef EMSCRIPTEN
#include <glad/glad.h>
#else
#include<emscripten/emscripten.h>
#define GLFW_INCLUDE_ES3
#endif
#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_styles.h"

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <cmath>

#include "global/globals.h"
#include "core/scene_object/scene_object.h"
#include "global/glbtime.h"
#include "editor/editor.h"
#include "core/shader/shader.h"
#include "core/mesh/mesh.h"
#include "core/resource/resource_manager.h"
#include "core/scene/scene_manager.h"
#include "components/mesh_renderer/mesh_renderer.h"
#include "components/camera/camera.h"

//DEBUG
Shader* shp;
Mesh* m;


GLFWwindow* winPtr = nullptr;

static double lastFrameTime = 0.0;

int screen[] = {960, 540};

void mainLoop();

int main(int argc, char** argv)
{
    //Set time this instance is running.
    Time::time = 0;
    //read commandline, to get all params
    for(int i = 0; i < argc; i++)
    {
        std::string option(argv[i]);
        //Now look at this beautiful if ladder 
        if(option == "--edit")
            Global::EditorMode = true;
    }

    //Now, check if we're in edit mode or in runtime, private version will use other approach
    if(Global::EditorMode)
    {
        printf("Launching editor application!\n");
    }
    
    //This is always called - initialize everything
    glfwInit();
    //glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    

    winPtr = glfwCreateWindow(screen[0], screen[1], "GLBenchy", nullptr, nullptr);
    glfwSetWindowSizeLimits(winPtr, 640, 480, GLFW_DONT_CARE, GLFW_DONT_CARE);
    glfwMakeContextCurrent(winPtr);

    #ifndef EMSCRIPTEN
    gladLoadGLES2Loader((GLADloadproc) glfwGetProcAddress);
    #endif

    int width, height;
    glfwGetFramebufferSize(winPtr, &width, &height);
    glViewport(0, 0, width, height);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    SetDarkStyle();

    ImGui_ImplGlfw_InitForOpenGL(winPtr, true);
    ImGui_ImplOpenGL3_Init("#version 300 es");

    printf("Entering main loop!\n");
    //Loop!
    //Set time offset
    Time::time = glfwGetTime();

    Scene* newSc = new Scene();

    m = new Mesh();

    m->vertices.push_back({0.5f, -0.5f, 0.0f});
    m->vertices.push_back({-0.5f, -0.5f, 0.0f});
    m->vertices.push_back({0.5f, 0.5f, 0.0f});
    m->vertices.push_back({-0.5f, 0.5f, 0.0f});

    m->colors.push_back({1.0f,0.0f,0.0f});
    m->colors.push_back({0.0f,1.0f,0.0f});
    m->colors.push_back({0.0f,0.0f,1.0f});
    m->colors.push_back({1.0f,0.0f,1.0f});

    m->indices.push_back(0);
    m->indices.push_back(1);
    m->indices.push_back(2);
    m->indices.push_back(1);
    m->indices.push_back(2);
    m->indices.push_back(3);

    m->CreateMesh();


    shp = ResourceManager::LoadShader("../assets/shader/default.vs", "../assets/shader/default.fs");


    GLBObject* newObj = new GLBObject();
    MeshRendererComponent* mr = new MeshRendererComponent();
    Material* mat = new Material();
    mat->m_Shader = shp;
    mr->m_Material = *mat;
    mr->m_Mesh = *m;
    newObj->AddComponent(mr);

    newSc->hierarchy[0] = newObj;

    newObj = new GLBObject();
    newObj->transform.Position(0.0f, 1.0f, -3.0f);
    CameraComponent* camera = new CameraComponent();
    newObj->AddComponent(camera);
    newObj->tags.insert("MainCamera");
    newSc->hierarchy[1] = newObj;

    SceneManager::scenes["scene1"] = newSc;
    SceneManager::activeScene = newSc;

    m = new Mesh();

    m->vertices.push_back({0.5f, -0.5f, 0.0f});
    m->vertices.push_back({-0.5f, -0.5f, 0.0f});
    m->vertices.push_back({0.5f, 0.5f, 0.0f});
    m->vertices.push_back({-0.5f, 0.5f, 0.0f});

    m->colors.push_back({0.0f,1.0f,0.0f});
    m->colors.push_back({1.0f,0.0f,0.0f});
    m->colors.push_back({0.0f,1.0f,1.0f});
    m->colors.push_back({1.0f,1.0f,0.0f});

    m->indices.push_back(0);
    m->indices.push_back(1);
    m->indices.push_back(2);
    m->indices.push_back(1);
    m->indices.push_back(2);
    m->indices.push_back(3);

    m->CreateMesh();

    newSc = new Scene();
    newObj = new GLBObject();
    mr = new MeshRendererComponent();
    mr->m_Material = *mat;
    mr->m_Mesh = *m;
    newObj->AddComponent(mr);
    newSc->hierarchy[0] = newObj;
    SceneManager::scenes["scene2"] = newSc;


    SceneManager::SwitchScene("scene1");

#ifdef EMSCRIPTEN
      emscripten_set_main_loop(mainLoop, 0, 1);
#else
    while (!glfwWindowShouldClose(winPtr))
    {
          mainLoop();
    }
#endif
    glfwTerminate();
    return 0;
}

bool shaderChooserOpen = true;

double changeTime = 5.0, changeTimer = changeTime;
bool curScene = false;

void mainLoop()
{
    //Calculate delta time.
    Time::time = glfwGetTime();
    Time::deltaTime = Time::time - lastFrameTime;
    lastFrameTime = Time::time;

    //begin ImGui Frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    //Rend
    ImGui::NewFrame();
    //Place for UI

    if(Global::EditorMode)
        Editor::DrawEditorUI();

    //Finalize UI
    ImGui::Render();
    glClear(GL_COLOR_BUFFER_BIT);
    //move to material
    shp->Use();
    SceneManager::activeScene->Update();
    SceneManager::activeScene->Draw();

    if(changeTimer <= 0.0)
    {
        SceneManager::SwitchScene((curScene ^= true) ? "scene2" : "scene1");
        changeTimer = changeTime;
    }


    //Render things?
    
    //Draw UI to framebuffer
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    //Swap Buffers.
    glfwSwapBuffers(winPtr);
    glfwPollEvents();

    if(changeTimer > 0.0)
        changeTimer -= Time::deltaTime;
}