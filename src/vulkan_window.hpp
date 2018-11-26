#pragma once

#define VK_USE_PLATFORM_WIN32_KHR

#define NUMBER_OF_QUEUE_INDICES 2

#include <vulkan/vulkan.h>
#include <algorithm>
#include <vector>
#include <array>
#include <set>
#include <optional>

#include "game.hpp"
#include "window.hpp"

namespace voxelfield::window {
    struct PhysicalDeviceInformation {
    public:
        VkPhysicalDevice handle;
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        std::vector<VkSurfaceFormatKHR> supportedSurfaceFormats;
        std::vector<VkPresentModeKHR> supportedPresentationModes;
        unsigned int score;
    };

    struct QueueFamilyIndices {
        uint32 graphicsFamilyIndex, presentationFamilyIndex;
    };

    class VulkanWindow : public Window {
    public:
        VulkanWindow(Application& application, const std::string& title);

        virtual ~VulkanWindow();

        bool Open() override;

    protected:
#ifdef VALIDATION_LAYERS_ENABLED
        VkDebugUtilsMessengerEXT m_DebugCallback;
        const std::vector<const char*> m_ValidationLayers;

        static VKAPI_ATTR VkBool32 VKAPI_CALL
        DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
                      const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData);

#endif
        const std::vector<const char*> m_RequiredExtensions, m_RequiredDeviceExtensions;
        VkInstance m_VulkanInstanceHandle;
        PhysicalDeviceInformation m_PhysicalDevice;
        QueueFamilyIndices m_QueueFamilyIndices;
        VkDevice m_LogicalDeviceHandle;
        VkQueue m_GraphicsQueueHandle;
        VkSurfaceKHR m_SurfaceHandle;
        VkSwapchainKHR m_SwapchainHandle;
        std::vector<VkImage> m_SwapchainImageHandles;
        VkFormat m_SwapchainImageFormat;
        VkExtent2D m_SwapchainExtent;
        std::vector<VkImageView> m_SwapchainImageViewHandles;

        void CreateVulkanInstance();

        void CreateSurface();

        void SelectPhysicalDevice();

        void CreateLogicalDevice();

        void CreateSwapChain();

        void CreateImageViews();
    };
}