#pragma once

#include <string>
#include <iostream>

#include "type_definitions.hpp"
#include "string_util.hpp"

#define MAX_MESSAGE_LENGTH 512

namespace voxelfield::logging {
    enum LogType : uint8 {
        INFO_LOG, WARNING_LOG, ERROR_LOG
    };

    void Log(LogType logType, const std::string& message);
}

