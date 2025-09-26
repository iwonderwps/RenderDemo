#pragma once

#include <vector>

#include "vector.h"

struct ImageScreenOutputSettings;

class Screen
{
public:
    using Color3 = Vector3i;
    using Color4 = Vector4i;

public:
    Screen();
    virtual ~Screen();

public:
    virtual void destroy();
    virtual void initialSetSize(int width, int height);

public:
    virtual int getScreenWidth() const;
    virtual int getScreenHeight() const;
    virtual void getScreenSize(size_t*pWidth, size_t* pHeight) const;

public:
    virtual void drawPointDirect(size_t x, size_t y, Color3& color);
    virtual void drawPointDirectSimple(size_t x, size_t y);

public:
    virtual void flush();
    virtual void clearBuffer();

public:
    virtual void resetOutputSettings(const ImageScreenOutputSettings& settings);

protected:
    virtual inline void checkCoord(size_t x, size_t y);
    virtual inline size_t getCoordIdx(size_t x, size_t y);

protected:
    int mWidth;
    int mHeight;
    const int mChannelNum;
    // +x 向屏幕右方  +y 向屏幕上方 （常规平面直角坐标系）
    std::vector<Color3> mBuffer;
};

