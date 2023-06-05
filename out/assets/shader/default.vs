#version 300 es

precision highp float;

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vColor;
layout (location = 2) in vec3 vNormal;
layout (location = 3) in vec2 uvs;
layout (location = 4) in vec3 tangent;
  

out vec3 FragPos;  
out vec2 TexCoord;
out vec3 WorldNormal;
out vec4 vertexColor;
out mat3 TBN;

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

uniform vec2 textureScaling;

void main()
{
    gl_Position = GLB_MVP * vec4(vPos, 1.0);
    vertexColor = vec4(vColor, 1.0);
    FragPos = vec3(GLB_M * vec4(vPos, 1.0));
    WorldNormal = mat3(GLB_ITM) * vNormal;
    TexCoord = uvs * textureScaling;
    vec3 T = normalize(vec3(GLB_M * vec4(tangent,   0.0)));
    vec3 N = normalize(vec3(GLB_M * vec4(vNormal,    0.0)));
    //T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    TBN = mat3(T,B,N);
}
