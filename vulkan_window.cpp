#include "vulkan_window.hpp"

namespace voxelfield::window {
    std::array<const char*, 2>VulkanWindow::s_Extensions{VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME};

    VulkanWindow::VulkanWindow(voxelfield::Application& application, const std::string& title) : Window(application, title) {
        CreateVulkanInstance();
        EnumeratePhysicalDevices();
        CreateLogicalDevice();
    }

    VulkanWindow::~VulkanWindow() {
        vkDestroyInstance(m_VulkanInstance, nullptr);
    }

    void VulkanWindow::CreateVulkanInstance() {
        VkApplicationInfo applicationInformation{
                VK_STRUCTURE_TYPE_APPLICATION_INFO,
                nullptr,
                m_Application.GetName().c_str(),
                VK_MAKE_VERSION(1, 0, 0),
                ENGINE_NAME,
                VK_MAKE_VERSION(1, 0, 0),
                VK_API_VERSION_1_1
        };
        VkInstanceCreateInfo instanceCreationInformation{
                VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                nullptr,
                0,
                &applicationInformation,
                0, nullptr,
                s_Extensions.size(), s_Extensions.data()
        };
        const VkResult result = vkCreateInstance(&instanceCreationInformation, nullptr, &m_VulkanInstance);
        if (result != VK_SUCCESS) {
            MessageBox(nullptr, util::Format("Error code %i, cannot create Vulkan 1.1 instance", MAX_MESSAGE_LENGTH, result).c_str(), m_Title.c_str(),
                       MB_OK);
            logging::Log(logging::LogType::ERROR_LOG,
                         util::Format("Error code %i was thrown when trying to create Vulkan 1.1 instance", MAX_MESSAGE_LENGTH, result));
            return;
        }
        logging::Log(logging::LogType::INFO_LOG, "Successfully created Vulkan 1.1 instance");
    }

    void VulkanWindow::EnumeratePhysicalDevices() {
        uint32 numberOfPhysicalDevices;
        vkEnumeratePhysicalDevices(m_VulkanInstance, &numberOfPhysicalDevices, nullptr);
        if (numberOfPhysicalDevices == 0) {
            const std::string errorMessage = "No graphics card detected capable of running Vulkan 1.1";
            MessageBox(nullptr, errorMessage.c_str(), m_Title.c_str(), MB_OK);
            logging::Log(logging::LogType::ERROR_LOG, errorMessage);
            return;
        }
        VkPhysicalDevice physicalDevicesHandles[numberOfPhysicalDevices];
        const VkResult result = vkEnumeratePhysicalDevices(m_VulkanInstance, &numberOfPhysicalDevices, physicalDevicesHandles);
        if (result != VK_SUCCESS) {
            const std::string errorMessage = util::Format("Error code %i, could not enumerate physical devices", MAX_MESSAGE_LENGTH, result);
            MessageBox(nullptr, errorMessage.c_str(), m_Title.c_str(), MB_OK);
            logging::Log(logging::LogType::ERROR_LOG, errorMessage);
            return;
        }
        PhysicalDevice physicalDevices[numberOfPhysicalDevices];
        unsigned int highestDeviceScore = 0, highestDeviceScoreIndex = 0;
        for (unsigned int deviceIndex = 0; deviceIndex < numberOfPhysicalDevices; deviceIndex++) {
            const VkPhysicalDevice& deviceHandle = physicalDevicesHandles[deviceIndex];
            VkPhysicalDeviceProperties deviceProperties;
            VkPhysicalDeviceFeatures deviceFeatures;
            vkGetPhysicalDeviceProperties(deviceHandle, &deviceProperties);
            vkGetPhysicalDeviceFeatures(deviceHandle, &deviceFeatures);
            const bool isIntegratedDevice = deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
            logging::Log(logging::LogType::INFO_LOG,
                         util::Format("Detected %s rendering device: %s", MAX_MESSAGE_LENGTH, isIntegratedDevice ? "integrated" : "discrete",
                                      deviceProperties.deviceName));
            const unsigned int deviceScore = isIntegratedDevice ? 0 : 1;
            physicalDevices[deviceIndex] = {
                    deviceHandle,
                    deviceProperties,
                    deviceFeatures,
                    deviceScore
            };
            if (deviceScore > highestDeviceScore) {
                highestDeviceScore = deviceScore;
                highestDeviceScoreIndex = deviceIndex;
            }
        }
        m_PhysicalDevice = physicalDevices[highestDeviceScoreIndex];
        logging::Log(logging::LogType::INFO_LOG,
                     util::Format("Using rendering device: %s", MAX_MESSAGE_LENGTH, m_PhysicalDevice.deviceProperties.deviceName));
    }

    void VulkanWindow::CreateLogicalDevice() {
        uint32 numberOfQueueFamilies;
        vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice.handle, &numberOfQueueFamilies, nullptr);
        VkQueueFamilyProperties queueFamilies[numberOfQueueFamilies];
        vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice.handle, &numberOfQueueFamilies, queueFamilies);
        uint32 wantedQueueFamilyIndex = 0;
        for (unsigned int queueFamilyIndex = 0; queueFamilyIndex < numberOfQueueFamilies; queueFamilyIndex++) {
            const VkQueueFamilyProperties& queueFamily = queueFamilies[queueFamilyIndex];
            if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                wantedQueueFamilyIndex = queueFamilyIndex;
                break;
            }
        }
        float priority = 1.0f;
        VkDeviceQueueCreateInfo deviceQueueCreateInfo{
                VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                nullptr,
                0,
                wantedQueueFamilyIndex,
                1,
                &priority
        };
        VkPhysicalDeviceFeatures physicalDeviceFeatures = {};
        VkDeviceCreateInfo deviceCreateInfo{
                VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                nullptr,
                0,
                1,
                &deviceQueueCreateInfo,
                0,
                nullptr,
                s_Extensions.size(), s_Extensions.data(),
                &physicalDeviceFeatures
        };
        VkResult result = vkCreateDevice(m_PhysicalDevice.handle, &deviceCreateInfo, nullptr, &m_LogicalDevice);
        if (result != VK_SUCCESS) {
            const std::string errorMessage = util::Format("Error code %i, could not create logical Vulkan 1.1 device", MAX_MESSAGE_LENGTH, result);
            logging::Log(logging::LogType::ERROR_LOG, errorMessage);
            MessageBox(nullptr, errorMessage.c_str(), m_Title.c_str(), MB_OK);
            return;
        }
        logging::Log(logging::LogType::INFO_LOG, "Successfully created logical Vulkan 1.1 device");
        vkGetDeviceQueue(m_LogicalDevice, wantedQueueFamilyIndex, 0, &m_GraphicsQueue);
    }
}
