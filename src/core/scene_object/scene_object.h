#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT_H

#include <cstdint>
#include <vector>
#include <unordered_set>

#include "../../global/uuid.h"
#include "../../components/component.h"
#include "../transform/transform.h"
#include "../object/object.h"

/**
 * Main class for objects used in GLB.
 * All objects have to derrive from this class to be properly initialized
*/
class GLBObject : public Object
{
public:
    GLBObject();
    std::string name;
    std::vector<GLBComponent*> components;
    std::unordered_set<std::string> tags;
    std::vector<GLBObject*> children;
    virtual void Update();
    virtual void Draw();
    Transform transform;
    bool disabled;
    void AddComponent(GLBComponent* componentPtr);

    template <typename T> GLBComponent* GetComponent()
    {
        for(auto& component : components)
        {
            if(dynamic_cast<T*>(component) != nullptr)
                return component;
        }
        return nullptr;
    }

    static GLBObject* FindWithTag(std::string tag);
};

#endif
