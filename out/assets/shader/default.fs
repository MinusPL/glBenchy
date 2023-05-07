#version 300 es

precision highp float;

in vec3 FragPos;  
in vec2 TexCoord;
in vec3 WorldNormal;
in vec4 vertexColor;

out vec4 FragColor;

uniform vec3 WorldSpaceCameraPos;
uniform vec3 mainLightDir;
uniform vec4 baseColor;

//Textures
uniform sampler2D Normal;
uniform sampler2D Albedo;

const float PI = 3.14159265359;

float specularStrength = 0.5;

vec3 lightColor = vec3(1.0,1.0,1.0);

void main()
{
    //FragColor = vertexColor; // set the output variable to a dark-red color
    vec3 norm = normalize(WorldNormal);
    vec3 lightDir = normalize(-mainLightDir);
    vec3 viewDir = normalize(WorldSpaceCameraPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * lightColor;  
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0,1.0,1.0);
    vec3 result = (vec3(0.2,0.34,0.40) + diffuse + specular) * baseColor.rgb * texture(Albedo, TexCoord).rgb;
    FragColor = vec4(result, 1.0);
}
