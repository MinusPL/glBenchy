#ifndef COMPONENT_H
#define COMPONENT_H

class GLBObject;

class GLBComponent
{
public:
    virtual void Update();
    GLBObject* mp_Object;
};

#endif