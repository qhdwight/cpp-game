#include "logger.hpp"

namespace voxelfield::logging {
    void Log(LogType logType, const std::string& message) {
        File* file = logType == LogType::ERROR_LOG ? stderr : stdout;
        std::fputs(message.c_str(), file);
        std::fputs("\n", file);
    }
}