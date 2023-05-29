#include "objloader.h"
#include "../core/resource/resource_manager.h"
#include "../components/arm_controller/armcontroller.h"
#include "../components/arm_controller/armjoint.h"

GLBObject *ObjLoader::CreateArm()
{
    GLBObject* armObj = ResourceManager::LoadModel("../assets/model/environment/factory/arm_uni.fbx");
    armObj->transform.Rotate(0.0f, 180.0f, 0.0f);
    //armObj->transform.Rotation(0.0f,0.0f,0.0f);
    //Add controller!
    //This WILL be messy.
    
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

    armJointPtr = new ArmJoint();
    armJointPtr->rotationAxis = {1.0f,0.0f,0.0f};
    lowerArmPtr->AddComponent(armJointPtr);
    accPtr->_joints.push_back(armJointPtr);

    armJointPtr = new ArmJoint();
    armJointPtr->rotationAxis = {1.0f,0.0f,0.0f};
    upperArmPtr->AddComponent(armJointPtr);
    accPtr->_joints.push_back(armJointPtr);

    armJointPtr = new ArmJoint();
    armJointPtr->rotationAxis = {1.0f,0.0f,0.0f};
    faceAttachPtr->AddComponent(armJointPtr);
    accPtr->_joints.push_back(armJointPtr);

    armJointPtr = new ArmJoint();
    armJointPtr->rotationAxis = {0.0f,1.0f,0.0f};
    faceRotatorPtr->AddComponent(armJointPtr);
    accPtr->_joints.push_back(armJointPtr);

    accPtr->_angles = std::vector<float>(accPtr->_joints.size(), 0.0f);
    
    //std::reverse(accPtr->_joints.begin(), accPtr->_joints.end());

    armObj->AddComponent(accPtr);
    return armObj;
}