#include <cstdio>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_styles.h"

#ifndef EMSCRIPTEN
#include <glad/glad.h>
#else
#include<emscripten/emscripten.h>
#define GLFW_INCLUDE_ES3
#endif
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"


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

#include "platform/platform.h"

#include "core/debug/debug.h"
#include "core/performance_monitor/performance_monitor.h"


GLFWwindow* winPtr = nullptr;

GLBObject* realCameraPtr = nullptr;

static double lastFrameTime = 0.0;

int screen[] = {1280, 720};

void mainLoop();

bool isRunning = true;

void exitMainLoop()
{
    std::cout <<"START EXIT CODE" <<std::endl;
#ifdef EMSCRIPTEN
    emscripten_cancel_main_loop();
#else
    glfwSetWindowShouldClose(winPtr, GLFW_TRUE);
#endif
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(winPtr);
}

void GLFWWindowSizeChanged(GLFWwindow* window, int width, int height)
{
    int vwidth, vheight;
    glfwGetFramebufferSize(winPtr, &vwidth, &vheight);
    glViewport(0, 0, vwidth, vheight);
}

void KeyInputs(GLFWwindow* window, int key, int scancode, int action, int mods);

//Screen framebuffer stuff
unsigned int fbo;
float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
    1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
    1.0f, -1.0f,  1.0f, 0.0f,
    1.0f,  1.0f,  1.0f, 1.0f
};

float skyboxVertices[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

unsigned int quadVAO, quadVBO;
unsigned int screenColorbuffers[2];

Shader* screenShader = nullptr;

Shader* blurShader = nullptr;
unsigned int pingpongFBO[2];
unsigned int pingpongColorbuffers[2];

unsigned int skyboxVAO, skyboxVBO;
unsigned int skyboxTexture;
Shader* skyboxShader = nullptr;

void initFramebuffer();

GLBObject* lightObj;

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
    glfwWindowHint(GLFW_RESIZABLE, false);
    //glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    

    winPtr = glfwCreateWindow(screen[0], screen[1], "GLBenchy",nullptr, nullptr);
    glfwSetWindowSizeLimits(winPtr, 640, 480, GLFW_DONT_CARE, GLFW_DONT_CARE);
    glfwMakeContextCurrent(winPtr);
    //glfwSwapInterval(0);
    glfwSetWindowSizeCallback(winPtr, GLFWWindowSizeChanged);
    glfwSetKeyCallback(winPtr, KeyInputs);

    #ifndef EMSCRIPTEN
    gladLoadGLES2Loader((GLADloadproc) glfwGetProcAddress);
    #endif

    int width, height;
    glfwGetFramebufferSize(winPtr, &width, &height);
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE); 

    
 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
    //glEnable(GL_MULTISAMPLE_ARB);
    glClearColor(0.4f, 0.2f, 0.6f, 1.0f);

    initFramebuffer();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = NULL;

    ImGui_ImplGlfw_InitForOpenGL(winPtr, true);
    ImGui_ImplOpenGL3_Init("#version 300 es");

    printf("Entering main loop!\n");

    //Still need to move it to other place, this is not supposed to be a part of "platform" code
    Scene* newSc = new Scene();
    SceneManager::scenes["scene1"] = newSc;

    //Load initial data, setup default resources.
    ResourceManager::defaultShader = ResourceManager::LoadShader("assets/shader/default.vs", "assets/shader/default.fs");
    ResourceManager::defaultMaterial = ResourceManager::LoadMaterial("assets/material/default.mat");

    //Move to init scene code!
    GLBObject* hallObj = ObjLoader::LoadHall();
    newSc->AddObject(hallObj);


    realCameraPtr = new GLBObject();
    realCameraPtr->transform.Position(-14.193f, 3.8731f, 35.386f);
    realCameraPtr->transform.Rotation(-7.0f,34.0f,0.0f);
    CameraComponent* camera = new CameraComponent();
    realCameraPtr->AddComponent(camera);
    realCameraPtr->tags.insert("MainCamera");
    newSc->AddObject(realCameraPtr);
    
    //SceneManager::activeScene = newSc;

    //Prep shader scene
    newSc = new Scene();
    GLBObject* oscPtr = new GLBObject();
    camera = new CameraComponent();
    camera->orthographicSize = {1, 1};
    camera->SetCameraMode(true);
    //camera->drawSkybox = false;
    camera->usePostprocess = false;
    oscPtr->AddComponent(camera);
    oscPtr->transform.Position(0.0f,0.0f, 2.0f);
    oscPtr->tags.insert("MainCamera");
    newSc->AddObject(oscPtr);
    oscPtr = ObjLoader::CreateWholeScreenQuad();
    newSc->AddObject(oscPtr);

    SceneManager::scenes["scene2"] = newSc;

    SceneManager::SwitchScene("scene1");

    //Debug::InitDebug();

    PerfMonitor::beginTest("Standard geometry test");
    glfwPollEvents();
    lastFrameTime = GetPlatformTime();
#ifdef EMSCRIPTEN
    emscripten_set_main_loop(mainLoop, 0, 0);
    emscripten_set_main_loop_timing(EM_TIMING_SETTIMEOUT, 0);
    glfwSwapInterval(0);
#else
    glfwSwapInterval(0);
    while (!glfwWindowShouldClose(winPtr))
    {
          mainLoop();
    }
    glfwTerminate();
#endif
    return 0;
}

bool shaderChooserOpen = true;

double changeTime = 5.0, changeTimer = changeTime;
bool curScene = false;

double runTime = 0.0f;
int runCounts = 0;
void mainLoop()
{
    double logicTime = GetPlatformTime();
    //Calculate delta time.
    Time::time = logicTime;
    //printf("%f\n", 1.0f/Time::deltaTime);


    SceneManager::activeScene->Update();

    //calc logic time
    logicTime = GetPlatformTime() - logicTime;

    //This is WRONG
    double renderTime = GetPlatformTime();
    //----------------------------

    //Draw scene to framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    SceneManager::activeScene->Draw();

    if(CameraComponent::current->drawSkybox)
    {
        //Only if scene is supposed to have skybox!
        //Draw skybox
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader->Use();
        HMM_Mat3 m3;
        m3.Columns[0] = CameraComponent::current->view.Columns[0].XYZ;
        m3.Columns[1] = CameraComponent::current->view.Columns[1].XYZ;
        m3.Columns[2] = CameraComponent::current->view.Columns[2].XYZ;
        UMat4 m4;
        m4.Columns[0] = {m3.Columns[0], 0.0f};
        m4.Columns[1] = {m3.Columns[1], 0.0f};
        m4.Columns[2] = {m3.Columns[2], 0.0f};
        m4.Columns[3] = {0.0f, 0.0f,0.0f, 1.0f};
        skyboxShader->SetMatrix4("GLB_V", CameraComponent::current->view);
        skyboxShader->SetMatrix4("GLB_P", CameraComponent::current->projection);
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);
    }

    bool horizontal = true;
    if(CameraComponent::current->usePostprocess)
    {
        //Only if scene is supposed to be post processed!
        //Post processing time
        horizontal = true;
        bool first_iteration = true;
        unsigned int amount = 10;
        blurShader->Use();
        for (unsigned int i = 0; i < amount; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            blurShader->SetInteger("horizontal", horizontal);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, first_iteration ? screenColorbuffers[1] : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6); 
            horizontal = !horizontal;
            if (first_iteration)
                first_iteration = false;
        }
    }

    //Draw framebuffer to window
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
    glDisable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
    glClear(GL_COLOR_BUFFER_BIT);
    screenShader->Use();
    screenShader->SetInteger("useBloom", (int)CameraComponent::current->usePostprocess);
    glBindVertexArray(quadVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screenColorbuffers[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    renderTime = GetPlatformTime() - renderTime;
    
    double uiTime = GetPlatformTime();
    //begin ImGui Frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    //Rend
    ImGui::NewFrame();
    //Place for UI
    PerfMonitor::DrawUI();
    //Finalize UI
    ImGui::Render();
    //Draw UI to framebuffer
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    uiTime = GetPlatformTime() - uiTime;
    //Swap Buffers.

    double GLFWTime = GetPlatformTime();
    glfwSwapBuffers(winPtr);
    glfwPollEvents();
    GLFWTime = GetPlatformTime() - GLFWTime;
    PerfMonitor::logicUpdateTime = logicTime;
    PerfMonitor::renderTime = renderTime;
    PerfMonitor::UITime = uiTime;
    PerfMonitor::glfwTime = GLFWTime;
    PerfMonitor::frameTime = Time::deltaTime;
    PerfMonitor::createSnapShot();
    Time::deltaTime = Time::time - lastFrameTime;
    lastFrameTime = Time::time;

    //Test controls
    if(runTime > 60.0)
    {
        runCounts++;
        if(runCounts >= 2)
        {
            PerfMonitor::saveTestResults();
            exitMainLoop();
        }
        SceneManager::SwitchScene("scene2");
        PerfMonitor::beginTest("Pure shader test");
        Time::time = GetPlatformTime();
        Time::deltaTime = 0.0;
        lastFrameTime = Time::time;
        runTime = 0.0;
    }
    runTime += Time::deltaTime;

}

void KeyInputs(GLFWwindow* window, int key, int scancode, int action, int mods)
{

}

void initFramebuffer()
{
    //Create screen quad
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    screenShader = ResourceManager::LoadShader("assets/shader/screen/screen.vs", "assets/shader/screen/screen.fs");
    screenShader->SetInteger("colorTexture", 0, true);
    screenShader->SetInteger("brightTexture", 1, true);

    blurShader = ResourceManager::LoadShader("assets/shader/screen/effect/blur.vs", "assets/shader/screen/effect/blur.fs");
    blurShader->SetInteger("image", 0, true);

    //Create framebuffers for HDR and bloom!
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);  

    glGenTextures(2, screenColorbuffers);
    for(int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, screenColorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screen[0], screen[1], 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, screenColorbuffers[i], 0);
    }
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo); 
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screen[0], screen[1]);  
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    
    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    // ping-pong-framebuffer for blurring
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorbuffers);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screen[0], screen[1], 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
        // also check if framebuffers are complete (no need for depth buffer)
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    skyboxShader = ResourceManager::LoadShader("assets/shader/skybox/default.vs", "assets/shader/skybox/default.fs");
    skyboxShader->SetInteger("skybox", 0, true);
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    skyboxTexture = ResourceManager::LoadCubemap("assets/texture/skybox/box1");
}