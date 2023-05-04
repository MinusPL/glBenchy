#version 300 es

precision highp float;

in vec3 FragPos;  
in vec3 Normal;
in vec4 vertexColor;

out vec4 FragColor;

uniform vec3 mainLightDir;

const float PI = 3.14159265359;

void main()
{
    //FragColor = vertexColor; // set the output variable to a dark-red color
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-mainLightDir); 
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0,1.0,1.0);
    vec3 result = (vec3(0.2,0.34,0.40) + diffuse) * vertexColor.rgb;
    FragColor = vec4(result, 1.0);
}
