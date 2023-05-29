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

uniform float _SinTime;

const float PI = 3.14159265359;

void main()
{
    vec4 texColor = texture(Albedo, TexCoord);
    if(texColor.a < 0.5)
        discard;
    FragColor = vec4(texColor.rgb * baseColor.rgb, 1.0);
}
