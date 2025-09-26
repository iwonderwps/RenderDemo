#include <ctime>
#include <sstream>
#include <iomanip>
#include "time_utility.h"

std::string TimeUtility::getCurTimeStr()
{
    auto t = std::time(nullptr);
    tm timeDesc;
    localtime_s(&timeDesc, &t);
    std::stringstream ssNameDefault;
    ssNameDefault << std::put_time(&timeDesc, "%Y-%m-%d_%H-%M-%S");
    return ssNameDefault.str();
}
