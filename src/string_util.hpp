#pragma once

#include <string>
#include <cstdarg>

namespace voxelfield::util {
    std::string Format(const std::string& format, unsigned int length, ...);
}