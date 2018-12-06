#pragma once

#include <vector>
#include <string>
#include <fstream>

#include "string_util.hpp"
#include "logger.hpp"

namespace voxelfield::file {
    static std::vector<char> ReadFile(const std::string& fileName);
}
