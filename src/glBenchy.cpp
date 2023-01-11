#include <cstdio>

#ifndef EMSCRIPTEN
#include <glad/glad.h>
#else
#include<emscripten/emscripten.h>
#define GLFW_INCLUDE_ES3
#endif
#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"


#include "global/globals.h"

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

#include <cmath>

//#include <unistd.h>

const char *vertexShaderSource = "#version 300 es\n"
    "layout (location = 0) in vec3 aPos;\n"         
    "layout (location = 1) in vec2 inTexCoord;\n"
    "out vec2 texCoord;\n"
    "void main()\n"
    "{\n"
    "   texCoord = inTexCoord;\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char* fragmentShaderSource2 = 
R"(#version 300 es

precision mediump float;

float t;
float aa;

in vec2 texCoord;
uniform vec2 iResolution;
uniform float iTime;
//uniform float iTime;
out vec4 fragColor;

void main() {
    fragColor = vec4(0.4, 0.2, 0.76,1.0);
})";

const char *fragmentShaderSource =
R"(#version 300 es

precision mediump float;

float t;
float aa;

in vec2 texCoord;
uniform vec2 iResolution;
uniform float iTime;
//uniform float iTime;
out vec4 fragColor;

mat2 rot(float a) { float c = cos(a), s = sin(a); return mat2(c,s,-s,c);}
float rand(vec2 st){ return fract(sin(dot(st.xy,vec2(12.9898,78.233)))*43758.585); }

// 2D Primitives
float sq(vec2 p, vec2 s, float r) { return length(max(abs(p) - s,0.)) - r;}

// df operations
float ext(float d, vec3 p, float h) {
    vec2 w = vec2(d,abs(p.z) - h);
    return min(max(w.x,w.y),0.) + length(max(w,0.));
}
float smoothunion( float d1, float d2, float k ) {
    float h = clamp( 0.5 + 0.5*(d2-d1)/k, 0.0, 1.0 );
    return mix( d2, d1, h ) - k*h*(1.0-h);
}

// 3D Primitives
float sph(vec3 p, float s) { return length(p) - s; }
float box(vec3 p, vec3 s, float r) { return length(max(abs(p) - s,0.)) - r;}
float cyl2( vec3 p, float h, float r ) {
    vec2 d = abs(vec2(length(p.xz),p.y)) - vec2(h,r);
    return max(length(max(abs(p.y) - h,0.)) - .01, length(p.xz) - r) - .01;
}
float hollowcyl( vec3 p, float h, float r ) {
    vec2 d = abs(vec2(length(p.xz),p.y)) - vec2(h,r);
    return max(length(max(abs(p.y) - h,0.)) - .01, abs(length(p.xz) - r) - .0) - .01;
}
float door(vec3 p, float s) {
    p *= s;
    float d2d = sq(p.xy,vec2(-.05,.515),.3);
    float d = ext(d2d,p,.2);
    return d/s;
}
float doors1(vec3 p, float s) {
    p *= s;
    float d = max(box(p,vec3(.9,1.,.15),.01),-door(p - vec3(0.,-1.3,(-0.525)),.4));
    p.x = abs(p.x);
    d = max(d,-door(p - vec3(0.3,-.5,(-0.1)),1.2));
    d = max(d,-box(p - vec3(0.3,-0.15,0.),vec3(.04,.08,.2),.001));
    d = max(d,-box(p - vec3(0.,0.35,0.),vec3(.04,.08,.2),.001));
    return d/s;
}
float towers1(vec3 p) {
    p.x -= .65;
    float d = hollowcyl(p,.51,.2) - .004*step(0.5,fract(1.5*p.y));
    vec3 p1 = p; p1.x = abs(p.x) - .02; p1.z += .1;
    d = max(d,-box(p1,vec3(.005,.02,5.),.001));
    d = max(d,-box(p1 + vec3(0.,.325,0.),vec3(.005,.02,5.),.001));
    d = max(d,-box(p1 - vec3(0.,.325,0.),vec3(.005,.02,5.),.001));
    p.y -= .5;
    d = smoothunion(d,sph(p,.19),.02);
    return d;
}

float towersAndDoors(vec3 p, float s) {
    p *= s;
    p.x = abs(p.x) - .375;
    float d1 = doors1(p*vec3(1.,1.,-1.),1.992);
    float d2 = towers1(p);
    float d = min(d1,d2);
    return ((d - .005*smoothstep(0.45,0.455,p.y)) + .0001*rand(floor(p.xy*vec2(.75,1.)*180.)))/s;
}

float buildings(vec3 p, float s) {
    float a = atan(p.x,p.z);
    float l = length(p.xz);
    vec3 rP = vec3(fract(a*3.03*s) - .5,fract(p.y*13.) - .5,l);
    float d = cyl2(p, .38,.2*s);
    d = max(d,-box(rP,vec3(.075,.15,10.),.001));
    d = min(d,sph((p - vec3(0.,0.4,0.))*vec3(1.,1.2,1.),.210*s));
    return d;
}

float df(vec3 p) {
    p.zy *= rot(cos(t*.2)*0. + 3.14*.25*.0 - .2);
    p.xz *= rot(t*.05);
    
    float a = atan(p.x,p.z);
    float l = length(p.xz);
    vec3 rP = vec3(fract(a*3.024) - .5,p.y,l - 2.5);
    vec3 rPfloor = vec3(floor(a*3.024) - .5,p.y,l - 2.5);
    vec3 rPnorm = vec3(a,p.y,l - 2.5);
    vec3 rP1 = vec3(fract((a + .2)*2.7058) - .5,p.y - .5,l - 2.2);
    vec3 rP2 = vec3(fract((a + .3)*2.285) - .5,p.y - 1.,l - 1.9);
    
    // 3 walls
    float d = 10e9;
    d = towersAndDoors(rP,2.065);
    d = max(d, -rP.y - (.25 - .01*rand(floor(rP.xz*150.)) - .04*rand(floor(rP.xz*20. + rPfloor.xz*20.)) + .02 ));
    d = min(d, towersAndDoors(rP1,2.065));
    d = min(d, towersAndDoors(rP2,2.065));
    
    // "grid" structure below
    float div = .35;
    vec3 pp; pp.y = rP.y; pp.xz = mod(rP.xz,div) - div*.5;
    float dGrid = max(box(pp,vec3(.2,.17 - .1*rand(floor(rP.xz*5.)),.2),.01), -box(pp,vec3(.15,1.,.15),.01));
    dGrid = max(dGrid,sph(p,2.45));
    d = min(d,dGrid);
    
    // main sphere
    vec3 pSph = p;
    pSph.y += .18;
    d = min(d,max(sph(pSph,1.6 - .025*rand(floor(rPnorm.xz*4.))),pSph.y - 0.44));
    
    // buildings
    p.y -= .25;
    vec3 rP3 = vec3(fract(a*1.1) - .5,p.y - 1.,l - 1.2);
    p.y -= .1;
    vec3 rP4 = vec3(fract((a*1.6 + 1.)*.25)*2. - .5,p.y - 1.,l - .8);
    vec3 rP5 = vec3(fract((a*.7 + 0.)) - .5,p.y - 1.,l - .7);
    p.y -= .1;
    vec3 rP6 = vec3(fract(a*.5) - .5,p.y - 1.,l - .5);
    d = min(d,buildings(rP3,.4));
    d = min(d,buildings(rP4,1.5));
    d = min(d,buildings(rP5,.8));
    d = min(d,buildings(rP6,.6));
    
    // huge tree
    float varTree = 12. + cos(t*.1);
    p.y += pow(length(p.xz)*.51,4.);
    d = min(d, sph((p - vec3(0.,1.9,0.))*vec3(1.,2.,1.) + fract(cos(p.x*varTree) + sin(p.y*varTree) + sin(p.y*varTree))*.032,1.25));
    
    return d;
}

#define E .001
vec3 normal(vec3 p) {
    vec2 u = vec2(0.,E); float d = df(p);
    return normalize(vec3(df(p + u.yxx),df(p + u.xyx),df(p + u.xxy)) - d);
}

#define MAX_D 60.
#define MIN_D 40.
#define MAX_STEP 30
#define EDP aa*10.
#define LIM .001
vec3 rm(vec3 cam, vec3 rayDir) {
    vec3 c = vec3(0.245,0.493,0.880) - rayDir.y*2.;
    vec3 pInit = cam + rayDir*MIN_D;
    vec3 p = pInit;
    float prevD = 10e9;
    for(int i = 0; i < MAX_STEP; i++) {
        float d = df(p);
        if(prevD < EDP && d > prevD) return c;
        if(d < LIM) {
            float dist = clamp(pow(distance(p,pInit)*.1,12.),0.,1.);
            vec3 n = normal(p);
            vec3 light = normalize(vec3(-1.095,1.204,-1.006));
            float lInf = clamp(dot(n,light),0.,1.);
            vec3 c1 = vec3(0.274,0.445,0.840)*dist*2. + vec3(0.605,0.581,0.080)*smoothstep(0.5-aa,.5+aa,lInf)-n;
            c = mix(c1,c,dist*.75);
            return c+.2;
        }
        if(distance(pInit,p) > MAX_D) return c;
        p += d*rayDir;
        prevD = min(prevD,d);
    }
    return c;
}

void main() {
    vec2 uv = gl_FragCoord.xy/iResolution.xy -.5;
    //uv = uv * 2.0 - 1.0;
    uv.x *= iResolution.x/iResolution.y;
    
    t = iTime;
	aa = 1./iResolution.x;

    vec3 c = vec3(0.,.55,-50.);
    vec3 r = normalize(vec3(uv,9.));
    vec3 col = rm(c,r);

    fragColor = vec4(col,1.0);
})";

const char* fragSource3 = 
R"(#version 300 es

precision mediump float;

float t;
float aa;

in vec2 texCoord;
uniform vec2 iResolution;
uniform float iTime;
//uniform float iTime;
out vec4 fragColor;

#define NUM_EXPLOSIONS 6.0
#define NUM_PARTICLES 100.0


vec2 Hash12(float t) {
  float x = fract(sin(t * 456.51) * 195.23);
  float y = fract(sin((t + x) * 951.2) * 462.1);
  return vec2(x, y);
}

vec2 Hash12_Polar(float t) {
  float a = fract(sin(t * 456.51) * 195.23) * 6.2832;
  float r = fract(sin((t + a) * 951.2) * 462.1);
  return vec2(sin(a), cos(a)) * r;

}

float Explosion(vec2 uv, float t) {
    float sparks = 0.0;
    for (float i=0.; i < NUM_PARTICLES; i++) {
        vec2 dir = Hash12_Polar(i + 1.0) * 0.5;
        float d = length(uv - dir * t);

        float brightness = mix(0.0003, 0.001, smoothstep(0.05, 0.0, t));
        brightness *= sin(t * 20.0 + i) * 0.5 + 0.5;
        brightness *= smoothstep(1.0, 0.7, t);
        sparks += brightness / d;
    }
    return sparks;
}

void main( )
{
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = (gl_FragCoord.xy - 0.5 * iResolution.xy) / iResolution.y;

    vec3 col = vec3(0.0);


    for ( float i = 0.0; i < NUM_EXPLOSIONS; i++) {
        float t = (iTime / 1.5) + (i * 123.4) / NUM_EXPLOSIONS;
        float ft = floor(t);
        vec3 colour = sin(4.0 * vec3(0.34, 0.54, 0.43) * ft) * 0.25 + 0.75;
        
        vec2 offset = Hash12(i + 1.0 + ft * NUM_EXPLOSIONS) - 0.5;
        offset *= vec2(0.9 * 1.777, 0.9);
        col += Explosion(uv - offset, fract(t)) * colour;
    }
    
    col *= 2.0;
    
    // Output to screen
    fragColor = vec4(col,1.0);
}
)";

unsigned int currentShader = 0;

unsigned int shaderProgram[16]; 
double lastFrame = 0;
GLFWwindow* winPtr = nullptr;

float screen[] = {960.0f, 540.0f};

unsigned int VBO, VAO, EBO;

float quadVerts[] = {
    -1.0, -1.0, 0.0,      0.0, 0.0,
    -1.0, 1.0,  0.0,      0.0, 1.0,
    1.0, -1.0,  0.0,      1.0, 0.0,

    1.0, -1.0, 0.0,       1.0, 0.0,
    -1.0, 1.0, 0.0,       0.0, 1.0,
    1.0, 1.0,  0.0,       1.0, 1.0
};

unsigned int indices[] = {  // note that we start from 0!
    0, 1, 3,  // first Triangle
    1, 2, 3   // second Triangle
};

uint16_t flags = 0;

enum shaders
{
    SHADER1 = 1,
    SHADER2 = 2,
    SHADER3 = 4
};

#include "object/object.h";
#include "global/glbtime.h"

    
GLBObject a;
GLBObject b;
GLBObject c;

void mainLoop();

//I was drunk or enlightened, either way when I wrote that me and god knew what this thing does.
//Now only god knows what is going on here.
//TODO: Cleanup and atomize parts. Introduce classes/structures.
int main(int argc, char** argv)
{
    //Set time this instance is running.
    Time::time = 0;

    //read commandline, to get all params
    for(int i = 0; i < argc; i++)
    {
        std::string option(argv[i]);
        //Now look at this beautiful if ladder 
        if(option == "--edit")
            Global::EditorMode = true;
    }

    //Now, check if we're in edit mode or in runtime, private version will use other approach
    if(Global::EditorMode)
    {
        printf("Launching editor application!\n");
        
    }


    printf("%s %s %s\n", UUIDGenerator::UUIDToString(a.id).c_str(),
                         UUIDGenerator::UUIDToString(b.id).c_str(),
                         UUIDGenerator::UUIDToString(c.id).c_str());
    
    //printf("%s\n", getcwd(NULL, 128));
    //Atomize it to modules etc.
    //This is always called - initialize everything
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    

    winPtr = glfwCreateWindow(screen[0], screen[1], "TEST", nullptr, nullptr);
    glfwMakeContextCurrent(winPtr);

    #ifndef EMSCRIPTEN
    gladLoadGLES2Loader((GLADloadproc) glfwGetProcAddress);
    #endif

    int width, height;
    glfwGetFramebufferSize(winPtr, &width, &height);
    glViewport(0, 0, width, height);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    //std::ifstream fragmentShaderFile("frag.fs");
    //std::stringstream fragmentStream;
    //fragmentStream << fragmentShaderFile.rdbuf();
    //fragmentShaderFile.close();
    //std::string fragmentCode = fragmentStream.str();
    //const char* fragmentCstr = fragmentCode.c_str();
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    shaderProgram[0] = glCreateProgram();
    glAttachShader(shaderProgram[0], vertexShader);
    glAttachShader(shaderProgram[0], fragmentShader);
    glLinkProgram(shaderProgram[0]);
    // check for linking errors
    glGetProgramiv(shaderProgram[0], GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram[0], 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(fragmentShader);

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource2, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    shaderProgram[1] = glCreateProgram();
    glAttachShader(shaderProgram[1], vertexShader);
    glAttachShader(shaderProgram[1], fragmentShader);
    glLinkProgram(shaderProgram[1]);
    // check for linking errors
    glGetProgramiv(shaderProgram[1], GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram[1], 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(fragmentShader);

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragSource3, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    shaderProgram[2] = glCreateProgram();
    glAttachShader(shaderProgram[2], vertexShader);
    glAttachShader(shaderProgram[2], fragmentShader);
    glLinkProgram(shaderProgram[2]);
    // check for linking errors
    glGetProgramiv(shaderProgram[2], GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram[2], 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(fragmentShader);

    glDeleteShader(vertexShader);

    currentShader = shaderProgram[0];
    flags = 1;

    glUseProgram(currentShader);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 
    glBindVertexArray(VAO);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(winPtr, true);
    ImGui_ImplOpenGL3_Init("#version 300 es");

    printf("Entering main loop!\n");
    //Loop!
    //Set time offset
    Time::time = glfwGetTime();
#ifdef EMSCRIPTEN
      emscripten_set_main_loop(mainLoop, 0, 1);
#else
    while (!glfwWindowShouldClose(winPtr))
    {
          mainLoop();
    }
#endif
    glfwTerminate();
    return 0;
}

bool shaderChooserOpen = true;

void drawShaderOptionWindow()
{
    ImGui::Begin("Choose shader program", &shaderChooserOpen);

    if(ImGui::CheckboxFlags("Use Shader 1", (unsigned int*)&flags, SHADER1))
    {
        flags &= ~(SHADER2 | SHADER3);
        currentShader = shaderProgram[0];
    }

    if(ImGui::CheckboxFlags("Use Shader 2", (unsigned int*)&flags, SHADER2))
    {
        flags &= ~(SHADER1 | SHADER3);
        currentShader = shaderProgram[1];
    }
        
    if(ImGui::CheckboxFlags("Use Shader 3", (unsigned int*)&flags, SHADER3))
    {
        flags &= ~(SHADER2 | SHADER1);
        currentShader = shaderProgram[2];
    }

    ImGui::End();
}

void mainLoop()
{
    //Calculate delta time.
    Time::time = glfwGetTime();
    Time::deltaTime = Time::time - lastFrame;
    lastFrame = Time::time;
    
    a.Update();

    //begin ImGui Frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    drawShaderOptionWindow();
    ImGui::Render();

    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(currentShader);
    glUniform2fv(glGetUniformLocation(currentShader, "iResolution"), 1, screen);
    glUniform1f(glGetUniformLocation(currentShader, "iTime"), fmod(Time::time, 60.f)); 
    glBindVertexArray(VAO);
    // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    glDrawArrays(GL_TRIANGLES, 0, 6);
    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(winPtr);
    glfwPollEvents();  
}