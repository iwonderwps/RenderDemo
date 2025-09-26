#pragma once

#include <string>

#include "screen.h"

struct ImageScreenOutputSettings;

class ImageScreen :public Screen
{
public:
    ImageScreen();
    virtual ~ImageScreen();

public:
    virtual void flush() override;

public:
    virtual void resetOutputSettings(const ImageScreenOutputSettings& settings);

protected:
    std::string mFolderPath;
    std::string mImageNamePrefix;
    size_t mImageNumber;
};

struct ImageScreenOutputSettings
{
    std::string startImageName = "";
    size_t startImageNumber = 0;
};