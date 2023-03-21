#version 300 es

precision highp float;

in vec4 vertexColor;

out vec4 FragColor;

void main()
{
    FragColor = vec4(0.5, 0.0, 0.0, 1.0); // set the output variable to a dark-red color
}