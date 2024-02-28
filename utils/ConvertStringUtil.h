// ConvertStringUtil.h

#pragma once

#include <string>
#include <sstream>

template<typename T>
T convertFromString(const std::string& str) {
    std::istringstream iss(str);
    T value;
    iss >> value;
    return value;
}
