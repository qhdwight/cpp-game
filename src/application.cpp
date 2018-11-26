#include "application.hpp"

namespace voxelfield {
    Application::Application(const std::string& name) {
        m_Name = name;
        m_Handle = GetModuleHandle(nullptr);
    }
}

