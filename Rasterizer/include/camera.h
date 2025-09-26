#pragma once

#include "matrix.h"
#include "vector.h"


class CameraConfig
{
public:
    CameraConfig() = default;
    virtual ~CameraConfig();
    CameraConfig(const CameraConfig&) = default;
    CameraConfig(CameraConfig&&) = default;
    CameraConfig(float n, float f, float fov, float aspect);

public:
    // 空间直角坐标系，右手系。相机空间，相机上方是+y，相机看向-z，位于原点。
    float mNear;
    float mFar;

    float mFovRad;
    float mAspectRatio;

public:
    virtual Matrix createProjectMatrixViewSpace() const;
    virtual Matrix createProjectMatrixClippingSpace() const;

public:
    CameraConfig& operator=(const CameraConfig& config);
    CameraConfig& operator=(CameraConfig&& config);

};


class Camera
{
public:
    Camera();
    virtual ~Camera();

public:
    Matrix ViewMatrix() const;
    Matrix ProjectionMatrix() const;

public:
    void SetCameraConfig(const CameraConfig& config);
    void SetCameraPosition(const Vector3f& position);
    void SetCameraFront(const Vector3f& front);
    void SetCameraUp(const Vector3f& up);
    Camera& LookAt(const Vector3f& target, const Vector3f& up);

public:
    Vector3f& GetPosition();

protected:
    CameraConfig mConfig;

protected:
    Vector3f mPosition;
    Vector3f mFront;
    Vector3f mUp;
    Vector3f mRight;
};