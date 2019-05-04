#pragma once

#define NUMBER_OF_QUEUE_INDICES 2
#define MAX_FRAMES_IN_FLIGHT 2

#include <vulkan/vulkan.h>
#include <algorithm>
#include <optional>
#include <vector>
#include <array>
#include <set>

#include "game.hpp"
#include "window.hpp"
#include "file_reader.hpp"

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

        void Open() override;

    protected:
        void Draw() override;

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
        VkQueue m_GraphicsQueueHandle, m_PresentationQueueHandle;
        VkSurfaceKHR m_SurfaceHandle;
        VkSwapchainKHR m_SwapchainHandle;
        VkRenderPass m_RenderPassHandle;
        std::vector<VkImage> m_SwapchainImageHandles;
        VkFormat m_SwapchainImageFormat;
        VkExtent2D m_SwapchainExtent;
        std::vector<VkImageView> m_SwapchainImageViewHandles;
        VkShaderModule m_VertexShaderModuleHandle, m_FragmentShaderModuleHandle;
        VkPipeline m_Pipeline;
        VkPipelineLayout m_PipelineLayoutHandle;
        std::vector<VkFramebuffer> m_SwapChainFramebufferHandles;
        VkCommandPool m_CommandPoolHandle;
        std::vector<VkCommandBuffer> m_CommandBufferHandles;
        std::vector<VkSemaphore> m_ImageAvailableSemaphoreHandles, m_RenderFinishedSemaphoreHandles;
        std::vector<VkFence> m_InFlightFences;
        size_t m_CurrentFrame = 0;

        void CreateVulkanInstance();

        void CreateSurface();

        void SelectPhysicalDevice();

        void CreateLogicalDevice();

        void CreateSwapChain();

        void CreateImageViews();

        void CreateGraphicsPipeline();

        void CreateRenderPass();

        void CreateFramebuffers();

        void CreateCommandPool();

        void CreateCommandBuffers();

        void CreateSynchronizationObjects();

        void DrawFrame();

        VkShaderModule CreateShaderModule(const std::vector<char>& shaderSource);
    };
}