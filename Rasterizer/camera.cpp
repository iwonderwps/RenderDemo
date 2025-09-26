#include <cmath>
#include "camera.h"

CameraConfig::~CameraConfig()
{
}

CameraConfig::CameraConfig(float n, float f, float fov, float aspect)
    :mNear(n), mFar(f), mFovRad(fov), mAspectRatio(aspect)
{
}

Matrix CameraConfig::createProjectMatrixViewSpace() const
{
    return Matrix::create({
        {mNear, 0, 0, 0},
        {0, mNear, 0, 0},
        {0, 0, mNear + mFar, -1 * mNear * mFar},
        {0, 0, 1, 0}
    });
}

Matrix CameraConfig::createProjectMatrixClippingSpace() const
{
    /*
    {1.0f / (-1 * this->aspectRatio * std::tan(this->fovRad * 0.5f)), 0, 0, 0},
    {0, 1.0f / (-1 * std::tan(this->fovRad * 0.5f)), 0, 0},
    {0, 0, (this->near + this->far) / (this->near - this->far), 2.0f * this->near * this->far / (this->far - this->near)},
    {0, 0, 1, 0}
    */
    float cot = 1.0f / std::tan(mFovRad / 2.0f);
    return Matrix::create({
        {-1.0f * cot / mAspectRatio,          0,                     0,                                        0               },
        {0,                             -1.0f * cot,                 0,                                        0               },
        {0,                                   0,        (mNear + mFar) / (mNear - mFar),     2.0f * mNear * mFar / (mFar - mNear)},
        {0,                                   0,                     1,                                        0               },
    });
}

CameraConfig& CameraConfig::operator=(const CameraConfig& config)
{
    mNear = config.mNear;
    mFar = config.mFar;
    mFovRad = config.mFovRad;
    mAspectRatio = config.mAspectRatio;
    return *this;
}

CameraConfig& CameraConfig::operator=(CameraConfig&& config)
{
    mNear = config.mNear;
    mFar = config.mFar;
    mFovRad = config.mFovRad;
    mAspectRatio = config.mAspectRatio;
    return *this;
}


Camera::Camera()
{
    mPosition = Vector3f::create({ 0, 0, 0 });
    mFront = Vector3f::create({ 0, 0, -1 });
    mUp = Vector3f::create({ 0, 1, 0 });
    mRight = (mFront % mUp).normalize();
}

Camera::~Camera()
{
}

Matrix Camera::ViewMatrix() const
{
    Matrix result = Matrix::create(4, 4);

    Vector3f forward = -1.0f * mFront;
    Vector3f right = mRight;
    Vector3f up = mUp;

    result(0, 0) = right[0], result(0, 1) = right[1], result(0, 2) = right[2], result(0, 3) = -1.0f * (right * mPosition);
    result(1, 0) = up[0], result(1, 1) = up[1], result(1, 2) = up[2], result(1, 3) = -1.0f * (up * mPosition);
    result(2, 0) = forward[0], result(2, 1) = forward[1], result(2, 2) = forward[2], result(2, 3) = -1.0f * (forward * mPosition);
    result(3, 0) = 0, result(3, 1) = 0, result(3, 2) = 0, result(3, 3) = 1;

    return result;
}

Matrix Camera::ProjectionMatrix() const
{
    return mConfig.createProjectMatrixClippingSpace();
}

void Camera::SetCameraConfig(const CameraConfig& config)
{
    mConfig = config;
}

void Camera::SetCameraPosition(const Vector3f& position)
{
    mPosition = position;
}

void Camera::SetCameraFront(const Vector3f& front)
{
    mFront = front;
}

void Camera::SetCameraUp(const Vector3f& up)
{
    mUp = up;
}

Camera& Camera::LookAt(const Vector3f& target, const Vector3f& up)
{
    if (!target.equals(mPosition))
    {
        mFront = (target - mPosition).normalize();
        mRight = (mFront % up).normalize();
        mUp = (mRight % mFront).normalize();
    }
    return *this;
}

Vector3f& Camera::GetPosition()
{
    return mPosition;
}
