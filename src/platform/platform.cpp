#include "platform.h"
#include "../global/globals.h"

#ifdef EMSCRIPTEN
#include<emscripten/emscripten.h>
#endif
#include <GLFW/glfw3.h>

void platformWindowResize(GLFWwindow* wPtr, int width, int height)
{
    if(Global::EditorMode)
    {
        
    }
    else
    {

    }
}

double GetPlatformTime()
{
#ifdef EMSCRIPTEN
    return emscripten_get_now()/1000.0;
#else
    return glfwGetTime();
#endif
}
