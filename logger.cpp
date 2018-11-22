#include "logger.hpp"


#include <iostream>
#include <vector>
#include <cstdarg>
#include <cstring>

namespace voxelfield::logging {
    void Logger::Log(LogType logType, const std::string message, ...) {
        char formattedMessage[MAX_MESSAGE_LENGTH];
        va_list arguments;
        va_start(arguments, message);
        int result = std::vsnprintf(formattedMessage, MAX_MESSAGE_LENGTH, message.c_str(), arguments);
        va_end(arguments);
        File* file = logType == LogType::ERROR_LOG ? stderr : stdout;
        std::fputs(formattedMessage, file);
    }
}