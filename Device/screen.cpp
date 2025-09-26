#include <cassert>

#include "screen.h"
#include "image_screen.h"

Screen::Screen():
    mWidth(0), mHeight(0), mChannelNum(3), mBuffer()
{
}

Screen::~Screen()
{
}

void Screen::destroy()
{
}

void Screen::initialSetSize(int width, int height)
{
    assert(width > 0 && height > 0);
    this->mWidth = width;
    this->mHeight = height;
    mBuffer.resize(size_t(width) * height);
}

void Screen::drawPointDirect(size_t x, size_t y, Color3& color)
{
    mBuffer.at(getCoordIdx(x, y)) = color;
}

void Screen::drawPointDirectSimple(size_t x, size_t y)
{
    mBuffer.at(getCoordIdx(x, y)) = Vector3i::create({ 255, 255, 255 });
}

void Screen::flush()
{
}

inline void Screen::checkCoord(size_t x, size_t y)
{
    assert(0 <= x && x < mWidth);
    assert(0 <= y && y < mHeight);
}

inline size_t Screen::getCoordIdx(size_t x, size_t y)
{
    this->checkCoord(x, y);
    return y * mWidth + x;
}

int Screen::getScreenWidth() const {
    return mWidth;
}

int Screen::getScreenHeight() const {
    return mHeight;
}

void Screen::getScreenSize(size_t* pWidth, size_t* pHeight) const {
    *pWidth = mWidth;
    *pHeight = mHeight;
}

void Screen::clearBuffer() {
    Color3 zero = std::move(Vector3i::zero());
    std::fill(mBuffer.begin(), mBuffer.end(), zero);
}

void Screen::resetOutputSettings(const ImageScreenOutputSettings& settings)
{
    // do nothing
}
