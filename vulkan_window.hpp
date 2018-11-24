#pragma once

#define VK_USE_PLATFORM_WIN32_KHR

#include <vulkan/vulkan.h>
#include <vector>
#include <array>
#include <map>
#include <optional>

#include "game.hpp"
#include "window.hpp"

namespace voxelfield::window {
    struct PhysicalDevice {
    public:
        VkPhysicalDevice handle;
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        unsigned int score;
    };

    class VulkanWindow : public Window {
    private:
        static std::array<const char*, 2> s_Extensions;
        VkInstance m_VulkanInstance;
        PhysicalDevice m_PhysicalDevice;
        VkDevice m_LogicalDevice;
        VkQueue m_GraphicsQueue;

        void CreateVulkanInstance();

        void EnumeratePhysicalDevices();

        void CreateLogicalDevice();

    public:
        VulkanWindow(Application& application, const std::string& title);

        virtual ~VulkanWindow();
    };
}