#version 300 es

precision highp float;

struct Light
{    
    vec4 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec4 color;
    //vec3 diffuse;
    //vec3 specular;
};  
#define LIGHTS_COUNT 8


in vec3 FragPos;  
in vec2 TexCoord;
in vec3 WorldNormal;
in vec4 vertexColor;

out vec4 FragColor;

uniform vec3 WorldSpaceCameraPos;
uniform vec3 mainLightDir;
uniform vec4 baseColor;

uniform int UsedLightCount;
uniform Light lights[LIGHTS_COUNT];

//Textures
uniform sampler2D Normal;
uniform sampler2D Albedo;

uniform float Shininess;

const float PI = 3.14159265359;

float specularStrength = 0.5;

vec3 ShadeLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 norm = normalize(WorldNormal);
    vec3 viewDir = normalize(WorldSpaceCameraPos - FragPos);
    vec3 lightning = vec3(0.0);
    
    vec3 result = vec3(0.0);

    for(int i = 0; i < UsedLightCount; i++)
    {
        result += ShadeLight(lights[i], norm, FragPos, viewDir);
    }
    FragColor = vec4(result, 1.0);
}

vec3 ShadeLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 ambient = vec3(0.0);
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);
    if(light.position.w > 0.0) //Directional light, no attenuation.
    {
        vec3 lightDir = normalize(-light.position.xyz);

        float diff = max(dot(normal, lightDir), 0.0);
        
        // specular shading
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), Shininess);
        
        // combine results
        ambient  = vec3(0.1, 0.15, 0.2) * texture(Albedo, TexCoord).rgb;
        diffuse  = light.color.rgb  * diff *  texture(Albedo, TexCoord).rgb/* * vec3(texture(material.diffuse, TexCoords))*/;
        specular = vec3(spec) /* * vec3(texture(material.specular, TexCoords))*/;
    }
    else //Non-directional light, ATTENUATE!
    {
        vec3 lightDir = normalize(light.position.xyz - fragPos);
        
        // diffuse shading
        float diff = max(dot(normal, lightDir), 0.0);
        
        // specular shading
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), Shininess);
        
        // attenuation
        float distance    = length(light.position.xyz - fragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + 
                    light.quadratic * (distance * distance));    
        // combine results
        ambient  = vec3(0.1, 0.15, 0.2) * texture(Albedo, TexCoord).rgb;
        diffuse  = light.color.rgb  * diff *  texture(Albedo, TexCoord).rgb/* * vec3(texture(material.diffuse, TexCoords))*/;
        specular = vec3(spec) /* * vec3(texture(material.specular, TexCoords))*/;
        ambient  *= attenuation;
        diffuse  *= attenuation;
        specular *= attenuation;
    }
    return (ambient + diffuse + specular) * baseColor.rgb;
}
