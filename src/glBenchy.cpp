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

#include "core/debug/debug.h"


GLFWwindow* winPtr = nullptr;

GLBObject* cameraObj;

float delta = 5.0f;
float rotAngle = 0.0f;
float rotAngle2 = 0.0f;
float rotDelta = 10.0f;
float rotDelta2 = 5.0f;

float rotAngle3 = 0.0f;

GLBObject* armObj, *armObj2;

static double lastFrameTime = 0.0;

int screen[] = {1280, 720};

void mainLoop();

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
unsigned int quadVAO, quadVBO;
unsigned int screenColorbuffers[2];

Shader* screenShader = nullptr;

Shader* blurShader = nullptr;
unsigned int pingpongFBO[2];
unsigned int pingpongColorbuffers[2];

void initFramebuffer();

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
    

    winPtr = glfwCreateWindow(screen[0], screen[1], "GLBenchy", nullptr, nullptr);
    glfwSetWindowSizeLimits(winPtr, 640, 480, GLFW_DONT_CARE, GLFW_DONT_CARE);
    glfwMakeContextCurrent(winPtr);
    glfwSwapInterval(0);
    glfwSetWindowSizeCallback(winPtr, GLFWWindowSizeChanged);
    glfwSetKeyCallback(winPtr, KeyInputs);

    #ifndef EMSCRIPTEN
    gladLoadGLES2Loader((GLADloadproc) glfwGetProcAddress);
    #endif

    int width, height;
    glfwGetFramebufferSize(winPtr, &width, &height);
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
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
    ResourceManager::LoadTexture("../assets/model/character/PC_A/textures/_04.png");
    GLBObject* newObj = ResourceManager::LoadModel("../assets/default/mesh/cube.fbx");
    GLBObject* plane = ResourceManager::LoadModel("../assets/default/mesh/plane.fbx");
    Material* mat = ResourceManager::LoadMaterial("../assets/material/unlit_cube.mat");

    plane->transform.Scale({150.0f,150.0f,150.0f});
    newSc->AddObject(plane);

    armObj = ResourceManager::LoadModel("../assets/model/environment/factory/arm.fbx");
    armObj->transform.Scale({0.25f,0.25f,0.25f});
    //armObj->transform.Scale({0.01f,0.01f,0.01f});
    //Add specific object loaders!
    armObj2 = ObjLoader::CreateArm();
    //armObj2->transform.Scale({0.25f,0.25f,0.25f});

    ((ArmControllerComponent*)armObj2->GetComponent<ArmControllerComponent>())->_targetPtr = newObj;

    armObj->transform.Position({-4.0f,0.0f,-3.0f});
    armObj2->transform.Position({0.0f,0.0f,-20.0f});
    //armObj2->transform.Rotation(HMM_QFromAxisAngle_RH({0.0,1.0f,0.0f}, HMM_AngleDeg(180.0f)));

    MeshRendererComponent* mr = (MeshRendererComponent*)newObj->GetComponent<MeshRendererComponent>();
    mr->m_Materials[0] = *mat;

    newObj->transform.Position({0.0f,6.0f,-20.0f});
    cameraObj = newObj; 
    newSc->AddObject(newObj);

    newObj = new GLBObject();
    newObj->transform.Position(0.0f, 3.0f, 3.0f);

    //newObj->transform.Rotation(HMM_QFromAxisAngle_RH({1.0f,0.0f,0.0f}, HMM_AngleDeg(90.0f)));
    newObj->transform.Rotation(-20.0f,0.0f,0.0f);
    
    CameraComponent* camera = new CameraComponent();
    newObj->AddComponent(camera);
    newObj->tags.insert("MainCamera");
    newSc->AddObject(newObj);

    newObj = new GLBObject();

    LightComponent* lightPtr = new LightComponent();

    lightPtr->lightData.type = DIRECTIONAL_LIGHT;
    lightPtr->lightData.color = {1.0f, 1.0f, 0.98f, 1.0f};
    lightPtr->lightData.spotAngle = 20.0f;
    lightPtr->lightData.softSpotAngle = 22.5f;
    lightPtr->lightData.linear = 0.007f;
    lightPtr->lightData.quadratic = 0.0002f;
    newObj->transform.Position({0.0f,20.0f,-20.0f});
    newObj->transform.Rotation(-55.0f, 15.0f, 0.0f);
    newObj->AddComponent(lightPtr);
    
    newSc->AddObject(newObj);

    newSc->AddObject(armObj);
    newSc->AddObject(armObj2);
    
    SceneManager::scenes["scene1"] = newSc;
    SceneManager::activeScene = newSc;

    lastFrameTime = Time::time;

    Debug::InitDebug();

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


    SceneManager::activeScene->Update();

    //-----------update additional logic
    double r = 10.0 * cos(Time::time*2.0);
    UVec3 newPos1 = {(float)(0.0 + (r * cos(Time::time))),6.0f + 4.0f * sin(Time::time/2.0),(float)(-20.0 + (r * sin(Time::time)))};
    cameraObj->transform.Position(newPos1);

    rotAngle += rotDelta * (float)Time::deltaTime;
    if(rotAngle > 360.0f) rotAngle -= 360.0f;
    cameraObj->transform.Rotation(HMM_QFromAxisAngle_RH({0.0,0.0f,1.0f}, HMM_AngleDeg(rotAngle)));

    rotAngle2 += rotDelta2 * (float)Time::deltaTime;
    if(rotAngle2 > 360.0f) rotAngle2 -= 360.0f;

    armObj->transform.Rotate(0.0f, 60.0f * (float)Time::deltaTime, 0.0f);
    if(armObj->transform.RotationEulerAngles().Y > 360.0f)
    {
        UVec3 rotAngles = armObj->transform.RotationEulerAngles();
        rotAngles.Y -= 360.0f;
        armObj->transform.Rotation(rotAngles);
    }
    //----------------------------

    //Draw scene to framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    SceneManager::activeScene->Draw();

    //BLOOM TIME
    bool horizontal = true, first_iteration = true;
    unsigned int amount = 10;
    blurShader->Use();
    for (unsigned int i = 0; i < amount; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
        blurShader->SetInteger("horizontal", horizontal);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, first_iteration ? screenColorbuffers[1] : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6); 
        horizontal = !horizontal;
        if (first_iteration)
            first_iteration = false;
    }

    //Draw framebuffer to window
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
    glDisable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
    glClear(GL_COLOR_BUFFER_BIT);

    screenShader->Use();
    glBindVertexArray(quadVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screenColorbuffers[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
    glDrawArrays(GL_TRIANGLES, 0, 6); 

    //Debug::DrawLine({0.0f,0.0f,0.0f}, {0.0f,1.0f,0.0f});

    //Render things?
    
    //Draw UI to framebuffer
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    //Swap Buffers.
    glfwSwapBuffers(winPtr);
    glfwPollEvents();
}

void KeyInputs(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_A && action == GLFW_PRESS) cameraObj->transform.Position(cameraObj->transform.Position() + HMM_V3(-1.0f,0.0f,0.0f));
    if(key == GLFW_KEY_D && action == GLFW_PRESS) cameraObj->transform.Position(cameraObj->transform.Position() + HMM_V3(1.0f,0.0f,0.0f));
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

    screenShader = ResourceManager::LoadShader("../assets/shader/screen/screen.vs", "../assets/shader/screen/screen.fs");
    screenShader->SetInteger("colorTexture", 0, true);
    screenShader->SetInteger("brightTexture", 1, true);

    blurShader = ResourceManager::LoadShader("../assets/shader/screen/effect/blur.vs", "../assets/shader/screen/effect/blur.fs");
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
}