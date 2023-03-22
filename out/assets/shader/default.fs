#version 300 es

precision highp float;

in vec4 vertexColor;

out vec4 FragColor;

void main()
{
    FragColor = vertexColor; // set the output variable to a dark-red color
}
