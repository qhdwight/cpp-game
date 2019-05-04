#include "string_util.hpp"

#include <vector>

namespace voxelfield::util {
    std::string Format(const std::string& format, const unsigned int length, ...) {
        va_list arguments;
        std::vector<char> output(length);
        va_start(arguments, length);
        std::vsnprintf(output.data(), length, format.c_str(), arguments);
        va_end(arguments);
        return output.data();
    }
}
