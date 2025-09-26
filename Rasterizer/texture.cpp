#include <iostream>
#include <cmath>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "texture.h"


Texture::Texture():
    mWidth(0), mHeight(0), mChannel(0)
{
}

Texture::~Texture()
{
}

Texture::Texture(const Texture& other)
{
    std::cout << "!! Texture::Texture(const Texture& other)" << std::endl;
    mWidth = other.mWidth;
    mHeight = other.mHeight;
    mChannel = other.mChannel;
    mData = other.mData;
}

Texture::Texture(Texture&& other) noexcept
{
    mWidth = other.mWidth;
    mHeight = other.mHeight;
    mChannel = other.mChannel;
    mData = std::move(other.mData);
}

Texture& Texture::operator=(const Texture& other)
{
    std::cout << "!! Texture& Texture::operator=(const Texture& other)" << std::endl;
    mWidth = other.mWidth;
    mHeight = other.mHeight;
    mChannel = other.mChannel;
    mData = other.mData;
    return *this;
}

Texture& Texture::operator=(Texture&& other) noexcept
{
    mWidth = other.mWidth;
    mHeight = other.mHeight;
    mChannel = other.mChannel;
    mData = std::move(other.mData);
    return *this;
}

void Texture::LoadTextureFromFile(const std::string& filePath)
{
    unsigned char* imgData = stbi_load(filePath.c_str(), &mWidth, &mHeight, &mChannel, 0);

    if (imgData)
    {
        // (u, v) 默认图片左上角是 (0, 0)  右下角是 (1, 1)
        size_t length = static_cast<size_t>(mWidth) * mHeight * mChannel;
        mData.resize(length);
        for (size_t i = 0; i < length; ++i)
        {
            mData[i] = static_cast<float>(imgData[i]) / 255;
        }
    }
    else 
    {
        std::cout << "failed to load " << filePath << std::endl;
    }

    stbi_image_free(imgData);
}

Vector3f Texture::Sample(float u, float v)
{
    if (u < 0 || u > 1 || v < 0 || v > 1)
    {
        return Vector3f::zero();
    }

    float tempFloat;
    float xFloat = u * (mWidth - 1), yFloat = v * (mHeight - 1);

    float xFrac = std::modf(xFloat, &tempFloat);
    int x1 = static_cast<int>(tempFloat);
    int x2 = static_cast<int>(std::ceil(xFloat));
    float yFrac = std::modf(yFloat, &tempFloat);
    int y1 = static_cast<int>(tempFloat);
    int y2 = static_cast<int>(std::ceil(yFloat));

    Vector3f colorV1 = GetVec3Value(x1, y1) * (1 - xFrac) + GetVec3Value(x2, y1) * xFrac;
    Vector3f colorV2 = GetVec3Value(x1, y2) * (1 - xFrac) + GetVec3Value(x2, y2) * xFrac;
    Vector3f color = colorV1 * (1 - yFrac) + colorV2 * yFrac;

    return color;
}

Vector3f Texture::GetVec3Value(int x, int y)
{
    size_t idx = (y * static_cast<size_t>(mWidth) + x) * mChannel;
    if (0 <= idx && idx + 2 < mData.size())
    {
        return Vector3f::create({ mData[idx], mData[idx + 1], mData[idx + 2] });
    }
    assert(false);
    return Vector3f::zero();
}
