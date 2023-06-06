#version 300 es

precision highp float;

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vColor;
layout (location = 2) in vec3 vNormal;
layout (location = 3) in vec2 uvs;
layout (location = 4) in vec3 tangent;
  
out vec3 FragPos;  
out vec3 TexCoord;

//Model, view, projection
uniform mat4 GLB_MVP;
uniform mat4 GLB_M;     //ObjectToWorld
uniform mat4 GLB_ITM;    //WorldToObject
uniform mat4 GLB_V;
uniform mat4 GLB_P;
//Camera Stuff!
uniform vec3 WorldSpaceCameraPos;

uniform mat4 model;
uniform vec4 baseColor;

void main()
{
    TexCoord = vPos;
    vec4 newPos = GLB_P * mat4(mat3(GLB_V)) * vec4(vPos, 1.0);
    gl_Position = newPos.xyww;
}
