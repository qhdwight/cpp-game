#pragma once

#include <string>

#include "type_definitions.hpp"

#define MAX_MESSAGE_LENGTH 512

namespace voxelfield::logging {
    enum LogType : uint8 {
        INFO_LOG, WARNING_LOG, ERROR_LOG
    };

    class Logger {
    public:
        static void Log(LogType logType, std::string message, ...);
    };
}

