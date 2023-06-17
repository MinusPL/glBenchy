#version 300 es
precision highp float;
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D colorTexture;
uniform sampler2D brightTexture;
uniform int useBloom;

const float offset = 1.0 / 300.0; 

float gamma = 2.2;
float exposure = 1.0;

const float kernel[9] = float[9](
    0.0, 0.0, 0.0,
    0.0, 0.0, 0.0,
    0.0, 0.0, 0.0
);

vec2 offsets[9] = vec2[](
    vec2(-offset,  offset), // top-left
    vec2( 0.0f,    offset), // top-center
    vec2( offset,  offset), // top-right
    vec2(-offset,  0.0f),   // center-left
    vec2( 0.0f,    0.0f),   // center-center
    vec2( offset,  0.0f),   // center-right
    vec2(-offset, -offset), // bottom-left
    vec2( 0.0f,   -offset), // bottom-center
    vec2( offset, -offset)  // bottom-right    
);

void main()
{
    // vec3 sampleTex[9];
    // for(int i = 0; i < 9; i++)
    // {
    //     sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]).rgb);
    // }
    // vec3 col = vec3(0.0);
    // for(int i = 0; i < 9; i++)
    //     col += sampleTex[i] * kernel[i];
    vec3 col = texture(colorTexture, TexCoords).rgb;
    vec3 bcol = texture(brightTexture, TexCoords).rgb;

    vec3 result = vec3(0.0);
    if(useBloom == 1)
    {
        vec3 resultColor = col+bcol;
        result = vec3(1.0) - exp(-resultColor * exposure);
        // also gamma correct while we're at it       
        result = pow(result, vec3(1.0 / gamma));
    }
    else
        result = col;

    FragColor = vec4(result, 1.0);
    // if(dot(col, vec3(0.2126, 0.7152, 0.0722)) > 1.0)
    //     FragColor = vec4(col, 1.0);
    // else
    //     FragColor = vec4(0.0,0.0,0.0,1.0);
}