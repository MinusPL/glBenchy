#include <cstdio>

#ifndef EMSCRIPTEN
#else
#include<emscripten/emscripten.h>
#define GLFW_INCLUDE_ES3
#endif

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_styles.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

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
#include "glbenchy/objloader.h"
#include "core/scene/scene_manager.h"
#include "components/mesh_renderer/mesh_renderer.h"
#include "components/camera/camera.h"
#include "components/light/light.h"
#include "components/arm_controller/armcontroller.h"


GLFWwindow* winPtr = nullptr;

GLBObject* cameraObj;
GLBObject* modelObj = nullptr;

float delta = 1.0f;
float rotAngle = 0.0f;
float rotAngle2 = 0.0f;
float rotDelta = 10.0f;
float rotDelta2 = 5.0f;

float rotAngle3 = 0.0f;

GLBObject* armObj, *armObj2;

static double lastFrameTime = 0.0;

int screen[] = {960, 540};

void mainLoop();

void GLFWWindowSizeChanged(GLFWwindow* window, int width, int height)
{
    int vwidth, vheight;
    glfwGetFramebufferSize(winPtr, &vwidth, &vheight);
    glViewport(0, 0, vwidth, vheight);
}

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
    glfwWindowHint(GLFW_SAMPLES, 4);
    

    winPtr = glfwCreateWindow(screen[0], screen[1], "GLBenchy", nullptr, nullptr);
    glfwSetWindowSizeLimits(winPtr, 640, 480, GLFW_DONT_CARE, GLFW_DONT_CARE);
    glfwMakeContextCurrent(winPtr);
    glfwSwapInterval(0);
    glfwSetWindowSizeCallback(winPtr, GLFWWindowSizeChanged);

    #ifndef EMSCRIPTEN
    gladLoadGLES2Loader((GLADloadproc) glfwGetProcAddress);
    #endif

    int width, height;
    glfwGetFramebufferSize(winPtr, &width, &height);
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);  
 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
    //glEnable(GL_MULTISAMPLE_ARB);
    glClearColor(0.4f, 0.2f, 0.6f, 1.0f);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = NULL;
    SetDarkStyle();

    ImGui_ImplGlfw_InitForOpenGL(winPtr, true);
    ImGui_ImplOpenGL3_Init("#version 300 es");

    printf("Entering main loop!\n");
    //Loop!
    //Set time offset
    Time::time = glfwGetTime();

    Scene* newSc = new Scene();

    //Load initial data, setup default resources.
    ResourceManager::defaultShader = ResourceManager::LoadShader("../assets/shader/default.vs", "../assets/shader/default.fs");
    ResourceManager::defaultMaterial = ResourceManager::LoadMaterial("../assets/material/default.mat");

    //Load default resources!
    modelObj = ResourceManager::LoadModel("../assets/model/character/PC_A/PC_A.fbx");
    modelObj->transform.Scale({0.01f,0.01f,0.01f});
    ResourceManager::LoadTexture("../assets/model/character/PC_A/textures/_04.png");
    GLBObject* newObj = ResourceManager::LoadModel("../assets/default/mesh/cube.fbx");
    Material* mat = ResourceManager::LoadMaterial("../assets/material/unlit_cube.mat");

    armObj = ResourceManager::LoadModel("../assets/model/environment/factory/arm.fbx");
    armObj->transform.Scale({0.25f,0.25f,0.25f});
    //armObj->transform.Scale({0.01f,0.01f,0.01f});
    //Add specific object loaders!
    armObj2 = ObjLoader::CreateArm();
    armObj2->transform.Scale({0.25f,0.25f,0.25f});

    ((ArmControllerComponent*)armObj2->GetComponent<ArmControllerComponent>())->_targetPtr = newObj;

    armObj->transform.Position({-2.0f,0.0f,3.0f});
    armObj2->transform.Position({2.0f,0.0f,3.0f});
    //armObj2->transform.Rotation(HMM_QFromAxisAngle_RH({0.0,1.0f,0.0f}, HMM_AngleDeg(180.0f)));

    MeshRendererComponent* mr = (MeshRendererComponent*)newObj->GetComponent<MeshRendererComponent>();
    mr->m_Materials[0] = *mat;

    newObj->transform.Position({0.0f,0.0f,3.0f});
    cameraObj = newObj; 
    newSc->AddObject(newObj);

    newObj = new GLBObject();
    newObj->transform.Position(0.0f, 1.0f, -1.0f);
    CameraComponent* camera = new CameraComponent();
    newObj->AddComponent(camera);
    newObj->tags.insert("MainCamera");
    newSc->AddObject(newObj);

    modelObj->transform.Position({0.0f,0.0f, 0.75f});
    //newSc->AddObject(modelObj);

    newObj = new GLBObject();
    newObj->transform.Position({0.0f,0.0f,1.0f});
    newObj->transform.Rotation(HMM_QFromAxisAngle_RH({1.0,0.0f,1.0f}, HMM_AngleDeg(60.0f)));

    LightComponent* lightPtr = new LightComponent();

    lightPtr->type = DIRECTIONAL_LIGHT;
    lightPtr->lightData.color = {1.0f, 1.0f, 0.98f, 1.0f};
    newObj->AddComponent(lightPtr);
    
    newSc->AddObject(newObj);

    newSc->AddObject(armObj);
    newSc->AddObject(armObj2);
    
    SceneManager::scenes["scene1"] = newSc;
    SceneManager::activeScene = newSc;

    lastFrameTime = Time::time;

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
    //printf("%f\n", 1.0f/Time::deltaTime);
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    SceneManager::activeScene->Update();
    SceneManager::activeScene->Draw();

    UVec3 deltaVec = {delta*((float)Time::deltaTime),0.0f,0.0f};
    cameraObj->transform.Position(cameraObj->transform.Position() + deltaVec);

    if(cameraObj->transform.Position().X > 3.0f && delta > 0.f) delta = -delta;
    if(cameraObj->transform.Position().X < -3.0f && delta < 0.f) delta = -delta;

    rotAngle += rotDelta * (float)Time::deltaTime;
    if(rotAngle > 360.0f) rotAngle -= 360.0f;
    cameraObj->transform.Rotation(HMM_QFromAxisAngle_RH({0.0,0.0f,1.0f}, HMM_AngleDeg(rotAngle)));

    rotAngle2 += rotDelta2 * (float)Time::deltaTime;
    if(rotAngle2 > 360.0f) rotAngle2 -= 360.0f;
    //modelObj->transform.Rotation(HMM_QFromAxisAngle_RH({0.0,1.0f,0.0f}, HMM_AngleDeg(rotAngle2)));

    //rotAngle3 += 360.0f * (float)Time::deltaTime;
    //if(rotAngle3 > 360.0f) rotAngle3 -= 360.0f;
    armObj->transform.Rotate(0.0f, 60.0f * (float)Time::deltaTime, 0.0f);
    if(armObj->transform.RotationEulerAngles().Y > 360.0f)
    {
        UVec3 rotAngles = armObj->transform.RotationEulerAngles();
        rotAngles.Y -= 360.0f;
        armObj->transform.Rotation(rotAngles);
    }
    //armObj2->transform.Rotation(HMM_QFromAxisAngle_RH({0.0f,1.0f,0.0f}, HMM_AngleDeg(rotAngle3-180.0f)));

    //Render things?
    
    //Draw UI to framebuffer
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    //Swap Buffers.
    glfwSwapBuffers(winPtr);
    glfwPollEvents();
}