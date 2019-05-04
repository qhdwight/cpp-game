#pragma once

#include <vector>
#include <string>
#include <fstream>

#include "string_util.hpp"
#include "logger.hpp"

namespace voxelfield::file {
    std::vector<char> ReadFile(const std::string& fileName);
}
