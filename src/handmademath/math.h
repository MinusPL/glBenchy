#ifndef GLBMATH_H
#define GLBMATH_H

#include "HandmadeMath.h"
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <cmath>

//Add type definitions

typedef HMM_Vec4 UVec4;
typedef HMM_Vec3 UVec3;
typedef HMM_Vec2 UVec2;
typedef HMM_Mat4 UMat4;
//typedef hmm_mat3 UMat3;
typedef HMM_Quat UQuat;

static inline UVec3 QuatToEuler(UQuat q)
{
    UVec3 angles;
    // roll (x-axis rotation)
    double sinr_cosp = 2 * (q.W * q.X + q.Y * q.Z);
    double cosr_cosp = 1 - 2 * (q.X * q.X + q.Y * q.Y);
    angles.Z = (float)atan2(sinr_cosp, cosr_cosp);

    // pitch (y-axis rotation)
    double sinp = std::sqrt(1 + 2 * (q.W * q.Y - q.X * q.Z));
    double cosp = std::sqrt(1 - 2 * (q.W * q.Y - q.X * q.Z));
    angles.X = (float)(2 * std::atan2(sinp, cosp) - M_PI / 2);

    // yaw (z-axis rotation)
    double siny_cosp = 2 * (q.W * q.Z + q.X * q.Y);
    double cosy_cosp = 1 - 2 * (q.Y * q.Y + q.Z * q.Z);
    angles.Y = (float)std::atan2(siny_cosp, cosy_cosp);

    return angles;
}

static inline  UQuat QuatFromEuler(UVec3 angles)
{
    // Abbreviations for the various angular functions

    double cr = cos(angles.Z * 0.5);
    double sr = sin(angles.Z * 0.5);
    double cp = cos(angles.X * 0.5);
    double sp = sin(angles.X * 0.5);
    double cy = cos(angles.Y * 0.5);
    double sy = sin(angles.Y * 0.5);

    UQuat q;
    q.W = (float)(cr * cp * cy + sr * sp * sy);
    q.X = (float)(sr * cp * cy - cr * sp * sy);
    q.Y = (float)(cr * sp * cy + sr * cp * sy);
    q.Z = (float)(cr * cp * sy - sr * sp * cy);
    return q;
}

static inline UQuat QuatFromEuler(float X, float Y, float Z)
{
    // Abbreviations for the various angular functions

    double cr = cos(Z * 0.5);
    double sr = sin(Z * 0.5);
    double cp = cos(X * 0.5);
    double sp = sin(X * 0.5);
    double cy = cos(Y * 0.5);
    double sy = sin(Y * 0.5);

    UQuat q;
    q.W = (float)(cr * cp * cy + sr * sp * sy);
    q.X = (float)(sr * cp * cy - cr * sp * sy);
    q.Y = (float)(cr * sp * cy + sr * cp * sy);
    q.Z = (float)(cr * cp * sy - sr * sp * cy);
    return q;
}

static inline UVec3 QMulV3(UQuat rotation, UVec3 point)
{
    float x = rotation.X * 2.f;
    float y = rotation.Y * 2.f;
    float z = rotation.Z * 2.f;
    float xx = rotation.X * x;
    float yy = rotation.Y * y;
    float zz = rotation.Z * z;
    float xy = rotation.X * y;
    float xz = rotation.X * z;
    float yz = rotation.Y * z;
    float wx = rotation.W * x;
    float wy = rotation.W * y;
    float wz = rotation.W * z;

    UVec3 res;
    res.X = (1.f - (yy + zz)) * point.X + (xy - wz) * point.Y + (xz + wy) * point.Z;
    res.Y = (xy + wz) * point.X + (1.f - (xx + zz)) * point.Y + (yz - wx) * point.Z;
    res.Z = (xz - wy) * point.X + (yz + wx) * point.Y + (1.f - (xx + yy)) * point.Z;
    return res;
}


#endif