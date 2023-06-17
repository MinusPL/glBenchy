#version 300 es

precision highp float;

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vColor;
layout (location = 2) in vec3 vNormal;
layout (location = 3) in vec2 uvs;
layout (location = 4) in vec3 tangent;

uniform mat4 GLB_MVP;
uniform mat4 GLB_M;     //ObjectToWorld

out vec3 FragPos;

void main()
{
    gl_Position = GLB_MVP * vec4(vPos, 1.0);
    FragPos = vec3(GLB_M * vec4(vPos, 1.0));
}