#ifndef COMPONENT_H
#define COMPONENT_H

class GLBObject;

class GLBComponent
{
public:
    GLBComponent();
    virtual void Start();
    virtual void Update();
    virtual void Draw();
    GLBObject* mp_Object;
    bool hasStarted;
};

#endif