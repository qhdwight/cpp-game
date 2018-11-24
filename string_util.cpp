#include "string_util.hpp"

namespace voxelfield::util {
    std::string Format(const std::string& format, unsigned int length, ...) {
        char formattedMessage[length];
        va_list arguments;
        va_start(arguments, length);
        auto formattedLength = static_cast<unsigned long>(std::vsnprintf(formattedMessage, length, format.c_str(), arguments));
        va_end(arguments);
        return std::string(formattedMessage, formattedLength);
    }
}
