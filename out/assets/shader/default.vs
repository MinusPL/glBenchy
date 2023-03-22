#version 300 es

precision highp float;

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vColor;
  
out vec4 vertexColor; // the input variable from the vertex shader (same name and same type)  

void main()
{
    gl_Position = vec4(vPos, 1.0); // see how we directly give a vec3 to vec4's constructor
    vertexColor = vec4(vColor, 1.0);
}
