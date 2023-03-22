#ifndef COMPONENT_H
#define COMPONENT_H

class GLBObject;

class GLBComponent
{
public:
    virtual void Update();
    virtual void Draw();
    GLBObject* mp_Object;
};

#endif