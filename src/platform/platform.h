#ifndef PLATFORM_H
#define PLATFORM_H

#ifndef EMSCRIPTEN
#include <glad/glad.h>
#else
#include<emscripten/emscripten.h>
#define GLFW_INCLUDE_ES3
#endif

#include <GLFW/glfw3.h>

void platformWindowResize(GLFWwindow* wPtr, int width, int height);

#endif