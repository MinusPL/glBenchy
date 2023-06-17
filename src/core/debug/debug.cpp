#include "debug.h"

#ifndef EMSCRIPTEN
#include <glad/glad.h>
#else
#include <emscripten/emscripten.h>
#define GLFW_INCLUDE_ES3
#endif
#include <GLFW/glfw3.h>

#include "../../components/camera/camera.h"

int Debug::line_shader;

void Debug::InitDebug()
{
    const char *vertexShaderSource = "#version 300 es\n"
        "precision highp float;\n"
        "layout (location = 0) in vec3 aPos;\n"
        "uniform mat4 MVP;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = MVP * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
    const char *fragmentShaderSource = "#version 300 es\n"
        "precision highp float;\n"
        "out vec4 FragColor;\n"
        "uniform vec4 color;\n"
        "void main()\n"
        "{\n"
        "   FragColor = color;\n"
        "}\n\0";

        // vertex shader
        int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);
        // check for shader compile errors

        // fragment shader
        int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);
        // check for shader compile errors

        // link shaders
        line_shader = glCreateProgram();
        glAttachShader(line_shader, vertexShader);
        glAttachShader(line_shader, fragmentShader);
        glLinkProgram(line_shader);
        // check for linking errors

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
}

void Debug::DrawLine(UVec3 start, UVec3 end, UVec4 color)
{
    std::vector<UVec3> vertices;
    vertices.push_back(start);
    vertices.push_back(end);
    GLuint lineVBO, lineVAO;
    glGenVertexArrays(1, &lineVAO);
    glGenBuffers(1, &lineVBO);
    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(UVec3), &(vertices[0]), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0);
    glUseProgram(line_shader);
    //Insert MVP
    UMat4 mvp = CameraComponent::current->projection * CameraComponent::current->view * HMM_M4D(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(line_shader, "MVP"), 1, GL_FALSE, (float*)&mvp);
    glUniform4f(glGetUniformLocation(line_shader, "color"), color.X, color.Y, color.Z, color.W);
    glBindVertexArray(lineVAO);
    glDrawArrays(GL_LINES, 0, 2);
    glDeleteVertexArrays(1, &lineVAO);
    glDeleteBuffers(1, &lineVBO);
}