#include "rasterizer.h"

#define _DELETE_POINTER(p) \
{\
    if (p)\
    {\
        delete (p);\
        (p) = nullptr;\
    }\
}

#define _RASTERIZER_SET_LINE_RESULT(screenX, screenY, tInterp) \
{\
    mLineResult->xScreen = (screenX);\
    mLineResult->yScreen = (screenY);\
    mLineResult->tInterpolate = (tInterp);\
}

#define _X_REF_NAME (*_px)
#define _Y_REF_NAME (*_py)
#define _RASTERIZER_DRAW_LINE_BASIC(x1Val, y1Val, x2Val, y2Val, xRefName, yRefName, pOutFuncVal) \
{\
    int _x1 = (x1Val), _x2 = (x2Val), _y1 = (y1Val), _y2 = (y2Val);\
    assert(_x1 < _x2 && _y1 <= _y2);\
    int dx = _x2 - _x1;\
    int dy = _y2 - _y1;\
    assert(float(dy) / dx <= 1.0f);\
\
    int* _px = &_x1;\
    int* _py = &_y1;\
\
    _RASTERIZER_SET_LINE_RESULT(xRefName, yRefName, 0);\
    pOutFuncVal(*mLineResult);\
\
    int D = 2 * dy - dx;\
    int x = _x1, y = _y1;\
    int len = _x2 - _x1;\
\
    for (int i = 1; i < dx; ++i) {\
        x += 1;\
        if (D <= 0) {\
            D += 2 * dy;\
        }\
        else {\
            D += 2 * (dy - dx);\
            y += 1;\
        }\
\
        _px = &x;\
        _py = &y;\
        _RASTERIZER_SET_LINE_RESULT(xRefName, yRefName, float(x - _x1) / len);\
        pOutFuncVal(*mLineResult);\
    }\
\
    _px = &_x2;\
    _py = &_y2;\
    _RASTERIZER_SET_LINE_RESULT(xRefName, yRefName, 1);\
    pOutFuncVal(*mLineResult);\
}

Rasterizer::Rasterizer():
    mLineResult(nullptr), mTriangleResult(nullptr),
    mXOffsetUnit(Vector2f::create({ 1.0f, 0.0f })),
    mYOffsetUnit(Vector2f::create({ 0.0f, 1.0f }))
{
    mLineResult = new RasterizerResultLine;
    mTriangleResult = new RasterizerResultTriangle;
}

Rasterizer::~Rasterizer()
{
    _DELETE_POINTER(mLineResult);
    _DELETE_POINTER(mTriangleResult);
}

void Rasterizer::DrawLine(const Vector4f& p1ndc, const Vector4f& p2ndc, const int screenWidth, const int screenHeight, const std::function<void(const RasterizerResultLine&)>& pOutFunc)
{
    assert(pOutFunc);
    int x1 = static_cast<int>((p1ndc[0] + 1) * 0.5f * screenWidth);
    int y1 = static_cast<int>((p1ndc[1] + 1) * 0.5f * screenHeight);
    int x2 = static_cast<int>((p2ndc[0] + 1) * 0.5f * screenWidth);
    int y2 = static_cast<int>((p2ndc[1] + 1) * 0.5f * screenHeight);

    const int dx = x2 - x1;
    const int dy = y2 - y1;
    const float k = dx != 0 ? static_cast<float>(dy) / dx : 0;

    if (dx == 0)
    {
        if (dy > 0)
        {
            const float len = static_cast<float>(y2 - y1);
            for (auto y = y1; y <= y2; ++y)
            {
                _RASTERIZER_SET_LINE_RESULT(x1, y, (y - y1) / len);
                pOutFunc(*mLineResult);
            }
        }
        else if (dy < 0)
        {
            const float len = static_cast<float>(y1 - y2);
            for (auto y = y1; y >= y2; --y)
            {
                _RASTERIZER_SET_LINE_RESULT(x1, y, (y1 - y) / len);
                pOutFunc(*mLineResult);
            }
        }
        else
        {
            _RASTERIZER_SET_LINE_RESULT(x1, y1, 0);
            pOutFunc(*mLineResult);
        }
    }
    else if (dx > 0)
    {
        if (k > 1)
        {
            _RASTERIZER_DRAW_LINE_BASIC(y1, x1, y2, x2, _Y_REF_NAME, _X_REF_NAME, pOutFunc);
        }
        else if (k >= 0)
        {
             _RASTERIZER_DRAW_LINE_BASIC(x1, y1, x2, y2, _X_REF_NAME, _Y_REF_NAME, pOutFunc);
        }
        else if (k >= -1)
        {
            _RASTERIZER_DRAW_LINE_BASIC(x1, -y1, x2, -y2, _X_REF_NAME, -_Y_REF_NAME, pOutFunc);
        }
        else
        {
            _RASTERIZER_DRAW_LINE_BASIC(-y1, x1, -y2, x2, _Y_REF_NAME, -_X_REF_NAME, pOutFunc);
        }
    }
    else // if (dx < 0)
    {
        if (k < -1)
        {
            _RASTERIZER_DRAW_LINE_BASIC(y1, -x1, y2, -x2, -_Y_REF_NAME, _X_REF_NAME, pOutFunc);
        }
        else if (k <= 0)
        {
            _RASTERIZER_DRAW_LINE_BASIC(-x1, y1, -x2, y2, -_X_REF_NAME, _Y_REF_NAME, pOutFunc);
        }
        else if (k <= 1)
        {
            _RASTERIZER_DRAW_LINE_BASIC(-x1, -y1, -x2, -y2, -_X_REF_NAME, -_Y_REF_NAME, pOutFunc);
        }
        else
        {
            _RASTERIZER_DRAW_LINE_BASIC(-y1, -x1, -y2, -x2, -_Y_REF_NAME, -_X_REF_NAME, pOutFunc);
        }
    }
}

float Rasterizer::InterpolateLineAttributeFloat(const float attr1, const float attr2, const RasterizerResultLine& rasterizerResult)
{
    return (1 - rasterizerResult.tInterpolate) * attr1 + rasterizerResult.tInterpolate * attr2;
}

void Rasterizer::DrawTiangle(const Vector4f* pTiangleVertexNdc, const int screenWidth, const int screenHeight, const std::function<void(const RasterizerResultTriangle&)>& pOutFunc)
{
    const Vector4f& p1Ndc = pTiangleVertexNdc[0];
    const Vector4f& p2Ndc = pTiangleVertexNdc[1];
    const Vector4f& p3Ndc = pTiangleVertexNdc[2];

    const float x1 = (p1Ndc[0] + 1) * 0.5f * screenWidth, y1 = (p1Ndc[1] + 1) * 0.5f * screenHeight;
    const float x2 = (p2Ndc[0] + 1) * 0.5f * screenWidth, y2 = (p2Ndc[1] + 1) * 0.5f * screenHeight;
    const float x3 = (p3Ndc[0] + 1) * 0.5f * screenWidth, y3 = (p3Ndc[1] + 1) * 0.5f * screenHeight;

    const float z1Projected = p1Ndc[2];
    const float z2Projected = p2Ndc[2];
    const float z3Projected = p3Ndc[2];

    const float z1ViewedInverse = 1.0f / p1Ndc[3];
    const float z2ViewedInverse = 1.0f / p2Ndc[3];
    const float z3ViewedInverse = 1.0f / p3Ndc[3];

    int xMin = static_cast<int>(std::min(screenWidth - 1.0f, std::max(0.0f, std::min(std::min(x1, x2), x3))));
    int yMin = static_cast<int>(std::min(screenHeight - 1.0f, std::max(0.0f, std::min(std::min(y1, y2), y3))));
    int xMax = static_cast<int>(std::min(screenWidth - 1.0f, std::max(0.0f, std::max(std::max(x1, x2), x3))));
    int yMax = static_cast<int>(std::min(screenHeight - 1.0f, std::max(0.0f, std::max(std::max(y1, y2), y3))));

    Vector2f p1Screen = Vector2f::create({ x1, y1 });
    Vector2f p2Screen = Vector2f::create({ x2, y2 });
    Vector2f p3Screen = Vector2f::create({ x3, y3 });

    Vector2f v1To2 = p2Screen - p1Screen;
    Vector2f v2To3 = p3Screen - p2Screen;
    Vector2f v3To1 = p1Screen - p3Screen;
    const float areaInverse = 1.0f / (v1To2 % static_cast<Vector2f>(-1.0f * v3To1));

    Vector2f minPointScreen = Vector2f::create({ static_cast<float>(xMin), static_cast<float>(yMin) });
    float areaTo1 = v2To3 % static_cast<Vector2f>(minPointScreen - p2Screen);
    float areaTo2 = v3To1 % static_cast<Vector2f>(minPointScreen - p3Screen);
    float areaTo3 = v1To2 % static_cast<Vector2f>(minPointScreen - p1Screen);

    // x y 方向 都是线性函数 可以计算单位增量
    const float dAreaTo1x = (v2To3 % static_cast<Vector2f>(minPointScreen + mXOffsetUnit - p2Screen)) - areaTo1;
    const float dAreaTo1y = (v2To3 % static_cast<Vector2f>(minPointScreen + mYOffsetUnit - p2Screen)) - areaTo1;
    const float dAreaTo2x = (v3To1 % static_cast<Vector2f>(minPointScreen + mXOffsetUnit - p3Screen)) - areaTo2;
    const float dAreaTo2y = (v3To1 % static_cast<Vector2f>(minPointScreen + mYOffsetUnit - p3Screen)) - areaTo2;
    const float dAreaTo3x = (v1To2 % static_cast<Vector2f>(minPointScreen + mXOffsetUnit - p1Screen)) - areaTo3;
    const float dAreaTo3y = (v1To2 % static_cast<Vector2f>(minPointScreen + mYOffsetUnit - p1Screen)) - areaTo3;

    // 遍历包围区域的每个像素  更快的方法是先遍历并过滤掉包围区域中在三角形外的子区域
    for (auto y = yMin; y <= yMax; ++y)
    {
        float curAreaTo1 = areaTo1, curAreaTo2 = areaTo2, curAreaTo3 = areaTo3;
        
        for (auto x = xMin; x <= xMax; ++x)
        {
            if (curAreaTo1 >= 0 && curAreaTo2 >= 0 && curAreaTo3 >= 0) // 在三角形内或边上
            {
                float aInScreen = curAreaTo1 * areaInverse, bInScreen = curAreaTo2 * areaInverse, cInScreen = curAreaTo3 * areaInverse;
                
                mTriangleResult->depthProjected = aInScreen * z1Projected + bInScreen * z2Projected + cInScreen * z3Projected;
                mTriangleResult->depthCorrected = 1.0f / (aInScreen * z1ViewedInverse + bInScreen * z2ViewedInverse + cInScreen * z3ViewedInverse);
                mTriangleResult->apzBary = aInScreen * z1ViewedInverse;
                mTriangleResult->bpzBary = bInScreen * z2ViewedInverse;
                mTriangleResult->cpzBary = cInScreen * z3ViewedInverse;

                mTriangleResult->xScreen = x;
                mTriangleResult->yScreen = y;

                pOutFunc(*mTriangleResult);
            }

            curAreaTo1 += dAreaTo1x;
            curAreaTo2 += dAreaTo2x;
            curAreaTo3 += dAreaTo3x;
        }

        areaTo1 += dAreaTo1y;
        areaTo2 += dAreaTo2y;
        areaTo3 += dAreaTo3y;
    }
}

float Rasterizer::InterpolateTriangleAttributeFloat(const float attr1, const float attr2, const float attr3, const RasterizerResultTriangle& rasterizerResult)
{
    return (attr1 * rasterizerResult.apzBary + attr2 * rasterizerResult.bpzBary + attr3 * rasterizerResult.cpzBary) * rasterizerResult.depthCorrected;
}

Vector4f Rasterizer::InterpolateTriangleAttributeVector4(const Vector4f& attr1, const Vector4f& attr2, const Vector4f& attr3, const RasterizerResultTriangle& rasterizerResult)
{
    return (attr1 * rasterizerResult.apzBary + attr2 * rasterizerResult.bpzBary + attr3 * rasterizerResult.cpzBary) * rasterizerResult.depthCorrected;
}

Vector3f Rasterizer::InterpolateTriangleAttributeVector3(const Vector3f& attr1, const Vector3f& attr2, const Vector3f& attr3, const RasterizerResultTriangle& rasterizerResult)
{
    return (attr1 * rasterizerResult.apzBary + attr2 * rasterizerResult.bpzBary + attr3 * rasterizerResult.cpzBary) * rasterizerResult.depthCorrected;
}

Vector2f Rasterizer::InterpolateTriangleAttributeVector2(const Vector2f& attr1, const Vector2f& attr2, const Vector2f& attr3, const RasterizerResultTriangle& rasterizerResult)
{
    return (attr1 * rasterizerResult.apzBary + attr2 * rasterizerResult.bpzBary + attr3 * rasterizerResult.cpzBary) * rasterizerResult.depthCorrected;
}

