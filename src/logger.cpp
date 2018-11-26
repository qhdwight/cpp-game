#include "logger.hpp"

namespace voxelfield::logging {
    void Log(LogType logType, const std::string& message) {
        (logType == LogType::ERROR_LOG ? std::cerr : std::cout) << message << std::endl;
    }
}