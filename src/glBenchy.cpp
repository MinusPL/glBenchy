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
#include "object/object.h"
#include "global/glbtime.h"
#include "editor/editor.h"
#include "core/shader/shader.h"
#include "core/mesh/mesh.h"

//DEBUG
Shader shp;
Mesh m;


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

    m.vertices.push_back({0.5f, -0.5f, 0.0f});
    m.vertices.push_back({0.5f, 0.5f, 0.0f});
    m.vertices.push_back({0.0f, 0.5f, 0.0f});

    m.colors.push_back({1.0f,1.0f,1.0f});
    m.colors.push_back({1.0f,1.0f,1.0f});
    m.colors.push_back({1.0f,1.0f,1.0f});

    m.indices.push_back(0);
    m.indices.push_back(1);
    m.indices.push_back(2);

    m.CreateMesh();

    {
        std::string vertexCode;
        std::string fragmentCode;
        std::string geometryCode;
        try
        {
            // Open files
            std::ifstream vertexShaderFile("../assets/shader/default.vs");
            std::ifstream fragmentShaderFile("../assets/shader/default.fs");
            std::stringstream vertexStream, fragmentStream;
            // Read file's buffer contents into streams
            vertexStream << vertexShaderFile.rdbuf();
            fragmentStream << fragmentShaderFile.rdbuf();
            // close file handlers
            vertexShaderFile.close();
            fragmentShaderFile.close();
            // Convert stream into string
            vertexCode = vertexStream.str();
            fragmentCode = fragmentStream.str();
        }
        catch (std::exception e)
        {
            std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
        }
        const GLchar *vertexShaderCode = vertexCode.c_str();
        const GLchar *fragmentShaderCode = fragmentCode.c_str();
        const GLchar *geometryShaderCode = geometryCode.c_str();

	    shp.Compile(vertexShaderCode, fragmentShaderCode, nullptr);
    }

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

    shp.Use();
    m.Draw();

    //Render things?
    
    //Draw UI to framebuffer
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    //Swap Buffers.
    glfwSwapBuffers(winPtr);
    glfwPollEvents();  
}