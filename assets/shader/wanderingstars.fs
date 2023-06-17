
#version 300 es

precision highp float;


in vec3 FragPos;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

uniform float _Time;

vec3 palette( in float t )
{
    vec3 a = vec3(0.5, 0.5, 0.5);
    vec3 b = vec3(0.5, 0.5, 0.5);
    vec3 c = vec3(1.0, 1.0, 1.0);
    vec3 d = vec3(0.00, 0.10, 0.20);
    
    return a + b*cos( 6.28318*(c*t+d) );
}

void main()
{
    // Normalize uv with a fixed aspect ratio
    vec2 uv = FragPos.xy;
    
    vec3 color = palette(uv.x + _Time / 4.0);
    
    float d0 = 0.0;
    for (float j = 1.0; j < 11.0; j++) { // Makes 5 groups of stars
        for (float i = 0.0; i < 100.0; i++) { // Makes 20 stars in every group
            // Bezier curve (https://www.youtube.com/watch?v=aVwxzDHniEw)
            float t = (sin(_Time + i*2.0) + 1.0) / 2.0;
            
            // 0.84, -0.67, 1.45 and so on are just random values to make the movement look random 
            vec2 p1 = vec2(sin(_Time * 0.83 * j), cos(_Time * -0.67 * j / 2.0)); // Multiplying by j to create the kind of flower effect
            vec2 p2 = vec2(cos(_Time * 1.45), cos(_Time * 0.78));
            vec2 p3 = vec2(cos(_Time * -0.67), sin(_Time * 0.23));
            vec2 p4 = vec2(sin(_Time * -0.19), sin(_Time * -0.35));
            vec2 p5 = p1;

            vec2 q1 = p1 + t * (p2 - p1);
            vec2 q2 = p2 + t * (p3 - p2);
            vec2 q3 = p3 + t * (p4 - p3);
            vec2 q4 = p4 + t * (p5 - p4);

            vec2 r1 = q1 + t * (q2 - q1);
            vec2 r2 = q2 + t * (q3 - q2);
            vec2 r3 = q3 + t * (q4 - q3);

            vec2 s1 = r1 + t * (r2 - r1);
            vec2 s2 = r2 + t * (r3 - r2);

            vec2 u1 = s1 + t * (s2 - s1);

            float d = length(uv - u1);

            d = 0.02 / d;

            if (d > d0) d0 = d;
        }
    }
    
    FragColor = vec4(color*d0, 1.0);
    BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}