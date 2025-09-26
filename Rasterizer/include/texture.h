#pragma once

#include <string>
#include <vector>
#include "vector.h"


class Texture
{
public:
    Texture();
    virtual ~Texture();

public:
    Texture(const Texture& other);
    Texture(Texture&& other) noexcept;

public:
    Texture& operator =(const Texture& other);
    Texture& operator =(Texture&& other) noexcept;

public:
    virtual void LoadTextureFromFile(const std::string& filePath);

public:
    virtual Vector3f Sample(float u, float v);

protected:
    virtual Vector3f GetVec3Value(int x, int y);

protected:
    std::vector<float> mData;
    int mWidth;
    int mHeight;
    int mChannel;
};