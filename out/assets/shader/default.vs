#version 300 es

precision highp float;

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vColor;
layout (location = 2) in vec3 vNormal;
layout (location = 3) in vec2 uvs;
layout (location = 4) in vec3 tangent;
  
out vec3 FragPos;  
out vec3 Normal;
out vec4 vertexColor; // the input variable from the vertex shader (same name and same type)  

uniform mat4 MVP;
uniform mat4 model;
uniform vec4 baseColor;

void main()
{
    gl_Position = MVP * vec4(vPos, 1.0); // see how we directly give a vec3 to vec4's constructor
    vertexColor = vec4(vColor, 1.0);
    FragPos = vec3(model * vec4(vPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * vNormal;
}
