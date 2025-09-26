#pragma once

#include <functional>

#include "vector.h"


struct RasterizerResult;
struct RasterizerResultLine;
struct RasterizerResultTriangle;

class Rasterizer
{
public:
    virtual ~Rasterizer();
    Rasterizer();

public:
    void DrawLine(const Vector4f& p1ndc, const Vector4f& p2ndc, const int screenWidth, const int screenHeight, const std::function<void(const RasterizerResultLine&)>& pOutFunc);
    float InterpolateLineAttributeFloat(const float attr1, const float attr2, const RasterizerResultLine& rasterizerResult);

public:
    void DrawTiangle(const Vector4f* pTiangleVertexNdc, const int screenWidth, const int screenHeight, const std::function<void(const RasterizerResultTriangle&)>& pOutFunc);
    
public:
    float InterpolateTriangleAttributeFloat(const float attr1, const float attr2, const float attr3, const RasterizerResultTriangle& rasterizerResult);
    Vector4f InterpolateTriangleAttributeVector4(const Vector4f& attr1, const Vector4f& attr2, const Vector4f& attr3, const RasterizerResultTriangle& rasterizerResult);
    Vector3f InterpolateTriangleAttributeVector3(const Vector3f& attr1, const Vector3f& attr2, const Vector3f& attr3, const RasterizerResultTriangle& rasterizerResult);
    Vector2f InterpolateTriangleAttributeVector2(const Vector2f& attr1, const Vector2f& attr2, const Vector2f& attr3, const RasterizerResultTriangle& rasterizerResult);

protected:
    RasterizerResultLine* mLineResult;
    RasterizerResultTriangle* mTriangleResult;

    const Vector2f mXOffsetUnit;
    const Vector2f mYOffsetUnit;
};


struct RasterizerResult
{
    int xScreen = 0;
    int yScreen = 0;
};

struct RasterizerResultLine : public RasterizerResult
{
    float tInterpolate = 0;
};

struct RasterizerResultTriangle : public RasterizerResult
{
    float depthProjected = 0; // 投影变换后的z，可用于比较相对深度
    float depthCorrected = 0; // 1 / Zt
    float apzBary = 0; // a / Za
    float bpzBary = 0; // b / Zb
    float cpzBary = 0; // c / Zc
};

