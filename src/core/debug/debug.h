#ifndef DEBUG_H
#define DEBUG_H

#include "../../handmademath/math.h"

#include <vector>

class Debug
{
    static int line_shader;
public:

    
    static void InitDebug();

    static void DrawLine(UVec3 start, UVec3 end, UVec4 color = {1.0f,1.0f,1.0f,1.0f});
};

#endif