#pragma once

#include <string>

#include "windows_definitions.hpp"

namespace voxelfield {
    class Application {
    private:
        std::string m_Name;
        ApplicationHandle m_Handle;
    public:
        Application(const std::string& name);

        ApplicationHandle GetHandle() {
            return m_Handle;
        }
        std::string& GetName() {
            return m_Name;
        }
    };
}
