#include "objloader.h"
#include "../core/resource/resource_manager.h"
#include "../components/arm_controller/armcontroller.h"
#include "../components/arm_controller/armjoint.h"
#include "../components/arm_controller/arm_target_mover.h"
#include "../core/scene/scene_manager.h"
#include "../components/light/light.h"
#include "../components/mesh_renderer/mesh_renderer.h"

GLBObject *ObjLoader::CreateArm()
{
    GLBObject* armObj = ResourceManager::LoadModel("assets/model/environment/factory/arm.fbx");
    //Get pointers to all children objects required
    GLBObject* rotBasePtr = nullptr, *lowerArmPtr = nullptr, *upperArmPtr = nullptr, *faceAttachPtr = nullptr, *faceRotatorPtr = nullptr;
    //I told ya, this thing going to be messy
    for(auto c : armObj->children)
        if(c->name.find("Joint") != std::string::npos)
        {
            rotBasePtr = c;
            break;
        }

    for(auto c : rotBasePtr->children)
        if(c->name.find("Joint") != std::string::npos)
        {
            lowerArmPtr = c;
            break;
        }

    for(auto c : lowerArmPtr->children)
        if(c->name.find("Joint") != std::string::npos)
        {
            upperArmPtr = c;
            break;
        }

    for(auto c : upperArmPtr->children)
        if(c->name.find("Joint") != std::string::npos)
        {
            faceAttachPtr = c;
            break;
        }

    for(auto c : faceAttachPtr->children)
        if(c->name == "ARM_FaceRotator")
        {
            faceRotatorPtr = c;
            break;
        }


    ArmControllerComponent* accPtr = new ArmControllerComponent();

    //now initialise each object joint
    ArmJoint* armJointPtr = new ArmJoint();
    armJointPtr->rotationAxis = {0.0f,1.0f,0.0f};
    rotBasePtr->AddComponent(armJointPtr);
    accPtr->_joints.push_back(armJointPtr);
    accPtr->_angleLimits.push_back({0.0f, 0.0f});

    armJointPtr = new ArmJoint();
    armJointPtr->rotationAxis = {1.0f,0.0f,0.0f};
    lowerArmPtr->AddComponent(armJointPtr);
    accPtr->_joints.push_back(armJointPtr);
    accPtr->_angleLimits.push_back({-90.0f, 90.0f});

    armJointPtr = new ArmJoint();
    armJointPtr->rotationAxis = {1.0f,0.0f,0.0f};
    upperArmPtr->AddComponent(armJointPtr);
    accPtr->_joints.push_back(armJointPtr);
    accPtr->_angleLimits.push_back({-150.0f, 150.0f});

    armJointPtr = new ArmJoint();
    armJointPtr->rotationAxis = {1.0f,0.0f,0.0f};
    faceAttachPtr->AddComponent(armJointPtr);
    accPtr->_joints.push_back(armJointPtr);
    accPtr->_angleLimits.push_back({-110.0f, 110.0f});

    armJointPtr = new ArmJoint();
    armJointPtr->rotationAxis = {0.0f,1.0f,0.0f};
    faceRotatorPtr->AddComponent(armJointPtr);
    accPtr->_joints.push_back(armJointPtr);
    accPtr->_angleLimits.push_back({-180.0f, 180.0f});

    accPtr->_angles = std::vector<float>(accPtr->_joints.size(), 0.0f);
    
    //std::reverse(accPtr->_joints.begin(), accPtr->_joints.end());

    armObj->AddComponent(accPtr);
    return armObj;
}

GLBObject *ObjLoader::LoadHall()
{
    GLBObject* hallObj = ResourceManager::LoadModel("assets/model/environment/factory/hall.fbx");
    GLBObject* targetObj = nullptr;
    GLBObject* p1,*p2;

    for(auto c : hallObj->children)
    {
        if(c->name == "CubePoint1")
            p1 = c;
        if(c->name == "CubePoint2")
            p2 = c;
    }

    for(auto c : hallObj->children)
        if(c->name == "TargetCube")
        {
            targetObj = c;
            ArmTargetMover* atmptr = new ArmTargetMover();
            atmptr->_point1 = p1;
            atmptr->_point2 = p2;
            atmptr->currentPoint = &p1->transform;
            targetObj->AddComponent(atmptr);
            break;
        }

    targetObj->transform.Position(targetObj->transform.Position().X, targetObj->transform.Position().Y, -20.0f);

    for(auto c : hallObj->children)
        if(c->name == "MovingArms")
        {   
            for(auto a : c->children)
            {
                GLBObject* armPtr = CreateArm();
                //armPtr->transform.Position(a->transform.Position());
                ((ArmControllerComponent*)armPtr->GetComponent<ArmControllerComponent>())->_targetPtr = targetObj;
                armPtr->transform.Position(a->transform.WorldPosition().X, a->transform.WorldPosition().Y, a->transform.WorldPosition().Z);
                SceneManager::scenes["scene1"]->AddObject(armPtr);

            }
        }
    //Add Lights!
    for(auto c : hallObj->children)
    {
        if(c->name.find("LightFix") != std::string::npos)
        {
            GLBObject* lightPtr = new GLBObject();
            lightPtr->transform.Position(c->transform.WorldPosition().X, c->transform.WorldPosition().Y - 1.0f, c->transform.WorldPosition().Z);
            lightPtr->transform.Rotation(-90.0f,0.0,0.0f);
            LightComponent* lcPtr = new LightComponent();
            lcPtr->lightData.color = {1.0f,1.0f,0.9f,1.0f};
            lcPtr->lightData.type = SPOT_LIGHT;
            lcPtr->lightData.spotAngle = 80.0f;
            lcPtr->lightData.softSpotAngle = 82.5f;
            lightPtr->AddComponent(lcPtr);
            SceneManager::scenes["scene1"]->AddObject(lightPtr);
        }
    }

    return hallObj;
}

GLBObject *ObjLoader::CreateWholeScreenQuad()
{
    GLBObject* quadObj = new GLBObject();
    MeshRendererComponent* mrPtr = new MeshRendererComponent();
    Mesh* meshResource = new Mesh();
    Surface* surfPtr = new Surface();

    surfPtr->vertices.push_back({-1.0f, 1.0f, 0.0f}); //TL
    surfPtr->vertices.push_back({1.0f, 1.0f, 0.0f}); //TR
    surfPtr->vertices.push_back({-1.0f, -1.0f, 0.0f}); //BL
    surfPtr->vertices.push_back({1.0f, -1.0f, 0.0f}); //BR

    surfPtr->indices.push_back(0);
    surfPtr->indices.push_back(3);
    surfPtr->indices.push_back(1);
    surfPtr->indices.push_back(0);
    surfPtr->indices.push_back(2);
    surfPtr->indices.push_back(3);

    surfPtr->CalculateTangents();
    meshResource->AddSurface(surfPtr);
    mrPtr->m_Mesh = meshResource;
    
    Material* mat = ResourceManager::LoadMaterial("assets/material/shadertoy.mat");
    mrPtr->m_Materials.push_back(*mat);

    quadObj->AddComponent(mrPtr);

    return quadObj;
}
