#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <array>

#include "game.hpp"
#include "window.hpp"

namespace voxelfield::window {
    class VulkanWindow : Window {
    private:
        VkInstance m_VulkanInstance;
    public:
        VulkanWindow(Application& application, const std::string& title);
    };
}