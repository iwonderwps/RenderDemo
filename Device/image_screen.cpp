#include <sstream>
#include "image_screen.h"
#include "stb_image_write.h"
#include "time_utility.h"

ImageScreen::ImageScreen()
    :mImageNamePrefix(), mImageNumber(0), mFolderPath()
{
    mImageNamePrefix = TimeUtility::getCurTimeStr();
    mFolderPath = "./Output";
}

ImageScreen::~ImageScreen()
{
}

void ImageScreen::flush()
{
    uint8_t* rgbImage;
    rgbImage = new uint8_t[size_t(mWidth) * mHeight * mChannelNum];

    size_t offset = 0;
    for (size_t yStb = 0; yStb < mHeight; ++yStb) {
        for (size_t xStb = 0; xStb < mWidth; ++xStb) {
            size_t xBuf = xStb, yBuf = mHeight - 1 - yStb;
            auto& color = mBuffer[getCoordIdx(xBuf, yBuf)];
            uint8_t r = static_cast<uint8_t>(color[0]), g = static_cast<uint8_t>(color[1]), b = static_cast<uint8_t>(color[2]);
            rgbImage[offset] = r;
            rgbImage[offset + 1] = g;
            rgbImage[offset + 2] = b;
            offset += 3;
        }
    }

    std::stringstream ssCurName;
    ssCurName << mFolderPath << "/" << mImageNamePrefix << "_" << mImageNumber++ << ".png";
    stbi_write_png(ssCurName.str().c_str(), mWidth, mHeight, mChannelNum, rgbImage, mWidth * mChannelNum);

    delete[] rgbImage;
    rgbImage = nullptr;
}

void ImageScreen::resetOutputSettings(const ImageScreenOutputSettings& settings)
{
    mImageNamePrefix = settings.startImageName;
    mImageNumber = settings.startImageNumber;
}
