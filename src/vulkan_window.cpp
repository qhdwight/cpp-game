#include "vulkan_window.hpp"

namespace voxelfield::window {
#ifdef VALIDATION_LAYERS_ENABLED

    VkBool32 VulkanWindow::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
                                         const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData) {
        logging::Log(
                messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT ? logging::LogType::ERROR_LOG : logging::LogType::INFORMATION_LOG,
                util::Format("[Vulkan Validation Layer][Severity %i] %s", MAX_MESSAGE_LENGTH, messageSeverity, callbackData->pMessage));
        return VK_FALSE;
    }

#endif

    VulkanWindow::VulkanWindow(Application& application, const std::string& title) : Window(application, title),
                                                                                     m_RequiredDeviceExtensions({
                                                                                                                        VK_KHR_SWAPCHAIN_EXTENSION_NAME
                                                                                                                }),
                                                                                     m_RequiredExtensions({
                                                                                                                  VK_KHR_SURFACE_EXTENSION_NAME,
                                                                                                                  VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#ifdef VALIDATION_LAYERS_ENABLED
                                                                                                                  ,
                                                                                                                  VK_EXT_DEBUG_UTILS_EXTENSION_NAME
#endif
                                                                                                          })
#ifdef VALIDATION_LAYERS_ENABLED
            , m_ValidationLayers({"VK_LAYER_LUNARG_standard_validation"})
#endif
    {
        CreateVulkanInstance();
    }

    VulkanWindow::~VulkanWindow() {
        vkDestroyPipelineLayout(m_LogicalDeviceHandle, m_PipelineLayoutHandle, nullptr);
        for (auto imageViewHandle : m_SwapchainImageViewHandles)
            vkDestroyImageView(m_LogicalDeviceHandle, imageViewHandle, nullptr);
        vkDestroySwapchainKHR(m_LogicalDeviceHandle, m_SwapchainHandle, nullptr);
        vkDestroyDevice(m_LogicalDeviceHandle, nullptr);
#ifdef VALIDATION_LAYERS_ENABLED
        auto destroyFunction = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(m_VulkanInstanceHandle,
                                                                                                           "vkDestroyDebugUtilsMessengerEXT"));
        if (destroyFunction) destroyFunction(m_VulkanInstanceHandle, m_DebugCallback, nullptr);
#endif
        vkDestroySurfaceKHR(m_VulkanInstanceHandle, m_SurfaceHandle, nullptr);
        vkDestroyInstance(m_VulkanInstanceHandle, nullptr);
    }

    bool VulkanWindow::Open() {
        bool success = Window::Open();
        CreateSurface();
        SelectPhysicalDevice();
        CreateLogicalDevice();
        CreateSwapChain();
        CreateImageViews();
        return success;
    }

    void VulkanWindow::CreateVulkanInstance() {
#ifdef VALIDATION_LAYERS_ENABLED
        uint32 numberOfLayers;
        vkEnumerateInstanceLayerProperties(&numberOfLayers, nullptr);
        VkLayerProperties availableLayerProperties[numberOfLayers];
        vkEnumerateInstanceLayerProperties(&numberOfLayers, availableLayerProperties);
        for (const char* layerName : m_ValidationLayers) {
            bool layerFound = false;
            for (const auto& layerProperties : availableLayerProperties) {
                if (!strcmp(layerName, layerProperties.layerName)) {
                    logging::Log(logging::LogType::INFORMATION_LOG,
                                 util::Format("Vulkan validation layer %s supported", MAX_MESSAGE_LENGTH, layerName));
                    layerFound = true;
                    break;
                }
            }
            if (!layerFound) {
                const std::string errorMessage = "Not all Vulkan validation layers supported!";
                MessageBox(nullptr, errorMessage.c_str(), m_Title.c_str(), MB_ICONERROR);
                logging::Log(logging::LogType::ERROR_LOG, errorMessage);
                return;
            }
        }
#endif
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
#ifdef VALIDATION_LAYERS_ENABLED
                static_cast<uint32>(m_ValidationLayers.size()), m_ValidationLayers.data(),
#else
                0, nullptr,
#endif
                static_cast<uint32>(m_RequiredExtensions.size()), m_RequiredExtensions.data()
        };
        if (const VkResult result = vkCreateInstance(&instanceCreationInformation, nullptr, &m_VulkanInstanceHandle); result != VK_SUCCESS) {
            const std::string errorMessage = util::Format("Error code %i, cannot create Vulkan instance", MAX_MESSAGE_LENGTH, result);
            MessageBox(nullptr, errorMessage.c_str(), m_Title.c_str(), MB_ICONERROR);
            logging::Log(logging::LogType::ERROR_LOG, errorMessage);
            return;
        }
        logging::Log(logging::LogType::INFORMATION_LOG, "Successfully created Vulkan instance");
#ifdef VALIDATION_LAYERS_ENABLED
        VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo{
                VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
                nullptr,
                0,
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
                DebugCallback,
                nullptr
        };
        if (auto createFunction = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(m_VulkanInstanceHandle,
                                                                                                             "vkCreateDebugUtilsMessengerEXT"));
                !createFunction || createFunction(m_VulkanInstanceHandle, &debugUtilsMessengerCreateInfo, nullptr, &m_DebugCallback) != VK_SUCCESS) {
            const std::string errorMessage = "Could not create Vulkan validation layer debug messenger";
            MessageBox(nullptr, errorMessage.c_str(), m_Title.c_str(), MB_ICONERROR);
            logging::Log(logging::LogType::ERROR_LOG, errorMessage);
            return;
        }
        logging::Log(logging::LogType::INFORMATION_LOG, "Successfully setup Vulkan validation layer debug messenger");
#endif
    }

    void VulkanWindow::CreateSurface() {
        VkWin32SurfaceCreateInfoKHR surfaceCreationInformation = {
                VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
                nullptr,
                0,
                m_Application.GetHandle(),
                m_Handle
        };
        if (VkResult result = vkCreateWin32SurfaceKHR(m_VulkanInstanceHandle, &surfaceCreationInformation, nullptr, &m_SurfaceHandle); result !=
                                                                                                                                       VK_SUCCESS) {
            const std::string errorMessage = util::Format("Error code %i, could not create windows rendering surface", MAX_MESSAGE_LENGTH, result);
            MessageBox(nullptr, errorMessage.c_str(), m_Title.c_str(), MB_ICONERROR);
            logging::Log(logging::LogType::ERROR_LOG, errorMessage);
        }
        logging::Log(logging::LogType::INFORMATION_LOG, "Successfully created windows rendering surface");
    }

    void VulkanWindow::SelectPhysicalDevice() {
        uint32 numberOfPhysicalDevices;
        vkEnumeratePhysicalDevices(m_VulkanInstanceHandle, &numberOfPhysicalDevices, nullptr);
        if (numberOfPhysicalDevices == 0) {
            const std::string errorMessage = "No graphics card detected capable of running Vulkan";
            MessageBox(nullptr, errorMessage.c_str(), m_Title.c_str(), MB_ICONERROR);
            logging::Log(logging::LogType::ERROR_LOG, errorMessage);
            return;
        }
        VkPhysicalDevice physicalDevicesHandles[numberOfPhysicalDevices];
        if (const VkResult result = vkEnumeratePhysicalDevices(m_VulkanInstanceHandle, &numberOfPhysicalDevices, physicalDevicesHandles); result !=
                                                                                                                                          VK_SUCCESS) {
            const std::string errorMessage = util::Format("Error code %i, could not enumerate physical devices", MAX_MESSAGE_LENGTH, result);
            MessageBox(nullptr, errorMessage.c_str(), m_Title.c_str(), MB_ICONERROR);
            logging::Log(logging::LogType::ERROR_LOG, errorMessage);
            return;
        }
        PhysicalDeviceInformation physicalDevices[numberOfPhysicalDevices];
        unsigned int highestDeviceScore = 0;
        std::optional<unsigned int> highestDeviceScoreIndex;
        for (unsigned int deviceIndex = 0; deviceIndex < numberOfPhysicalDevices; deviceIndex++) {
            const VkPhysicalDevice& deviceHandle = physicalDevicesHandles[deviceIndex];
            VkPhysicalDeviceProperties deviceProperties;
            VkPhysicalDeviceFeatures deviceFeatures;
            vkGetPhysicalDeviceProperties(deviceHandle, &deviceProperties);
            vkGetPhysicalDeviceFeatures(deviceHandle, &deviceFeatures);
            // Check if required extensions are supported
            bool areRequiredCapabilitiesSupported = true;
            uint32 extensionCount;
            vkEnumerateDeviceExtensionProperties(deviceHandle, nullptr, &extensionCount, nullptr);
            std::vector<VkExtensionProperties> availableExtensions(extensionCount);
            vkEnumerateDeviceExtensionProperties(deviceHandle, nullptr, &extensionCount, availableExtensions.data());
            for (const char* requiredExtensionName : m_RequiredDeviceExtensions) {
                bool extensionFound = false;
                for (const VkExtensionProperties& availableExtensionProperties : availableExtensions) {
                    if (!strcmp(requiredExtensionName, availableExtensionProperties.extensionName)) {
                        logging::Log(logging::LogType::INFORMATION_LOG,
                                     util::Format("Vulkan extension %s supported for device %s", MAX_MESSAGE_LENGTH, requiredExtensionName,
                                                  deviceProperties.deviceName));
                        extensionFound = true;
                        break;
                    }
                }
                if (!extensionFound) {
                    const std::string errorMessage = util::Format("Not all Vulkan device extensions supported for device %s", MAX_MESSAGE_LENGTH,
                                                                  deviceProperties.deviceName);
                    MessageBox(nullptr, errorMessage.c_str(), m_Title.c_str(), MB_ICONERROR);
                    logging::Log(logging::LogType::ERROR_LOG, errorMessage);
                    areRequiredCapabilitiesSupported = false;
                    break;
                }
            }
            // Check for surface capability
            VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
            if (const VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(deviceHandle, m_SurfaceHandle, &surfaceCapabilities); result !=
                                                                                                                                        VK_SUCCESS) {
                const std::string errorMessage = util::Format("Error code %i, could not retrieve device surface capabilities", MAX_MESSAGE_LENGTH,
                                                              result);
                MessageBox(nullptr, errorMessage.c_str(), m_Title.c_str(), MB_ICONERROR);
                logging::Log(logging::LogType::ERROR_LOG, errorMessage);
                return;
            }
            uint32 numberOfFormats;
            vkGetPhysicalDeviceSurfaceFormatsKHR(deviceHandle, m_SurfaceHandle, &numberOfFormats, nullptr);
            if (numberOfFormats == 0) {
                areRequiredCapabilitiesSupported = false;
                logging::Log(logging::LogType::ERROR_LOG,
                             util::Format("No image formats supported for device %s", MAX_MESSAGE_LENGTH, deviceProperties.deviceName));
            }
            std::vector<VkSurfaceFormatKHR> supportedSurfaceFormats(numberOfFormats);
            vkGetPhysicalDeviceSurfaceFormatsKHR(deviceHandle, m_SurfaceHandle, &numberOfFormats, supportedSurfaceFormats.data());
            uint32 numberOfPresentationModes;
            vkGetPhysicalDeviceSurfacePresentModesKHR(deviceHandle, m_SurfaceHandle, &numberOfPresentationModes, nullptr);
            if (numberOfPresentationModes == 0) {
                areRequiredCapabilitiesSupported = false;
                logging::Log(logging::LogType::ERROR_LOG,
                             util::Format("No presentation modes supported for device %s", MAX_MESSAGE_LENGTH, deviceProperties.deviceName));
            }
            std::vector<VkPresentModeKHR> supportedPresentationModes(numberOfPresentationModes);
            vkGetPhysicalDeviceSurfacePresentModesKHR(deviceHandle, m_SurfaceHandle, &numberOfPresentationModes,
                                                      supportedPresentationModes.data());
            const bool isIntegratedDevice = deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
            logging::Log(logging::LogType::INFORMATION_LOG,
                         util::Format("Detected %s rendering device: %s", MAX_MESSAGE_LENGTH, isIntegratedDevice ? "integrated" : "discrete",
                                      deviceProperties.deviceName));
            const unsigned int deviceScore = isIntegratedDevice ? 0 : 1;
            physicalDevices[deviceIndex] = {
                    deviceHandle,
                    deviceProperties,
                    deviceFeatures,
                    surfaceCapabilities,
                    supportedSurfaceFormats,
                    supportedPresentationModes,
                    deviceScore
            };
            if (areRequiredCapabilitiesSupported && deviceScore > highestDeviceScore) {
                highestDeviceScore = deviceScore;
                highestDeviceScoreIndex = deviceIndex;
            }
        }
        if (!highestDeviceScoreIndex.has_value()) {
            const std::string errorMessage = "No graphics card detected with suitable Vulkan function requirements";
            MessageBox(nullptr, errorMessage.c_str(), m_Title.c_str(), MB_ICONERROR);
            logging::Log(logging::LogType::ERROR_LOG, errorMessage);
            return;
        }
        m_PhysicalDevice = physicalDevices[highestDeviceScoreIndex.value()];
        logging::Log(logging::LogType::INFORMATION_LOG,
                     util::Format("Using rendering device: %s", MAX_MESSAGE_LENGTH, m_PhysicalDevice.deviceProperties.deviceName));
    }

    void VulkanWindow::CreateLogicalDevice() {
        uint32 numberOfQueueFamilies;
        vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice.handle, &numberOfQueueFamilies, nullptr);
        VkQueueFamilyProperties queueFamilies[numberOfQueueFamilies];
        vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice.handle, &numberOfQueueFamilies, queueFamilies);
        bool hasRequiredQueueFamilies = false;
        std::optional<uint32> graphicsFamilyIndex, presentationFamilyIndex;
        for (unsigned int queueFamilyIndex = 0; queueFamilyIndex < numberOfQueueFamilies; queueFamilyIndex++) {
            const VkQueueFamilyProperties& queueFamily = queueFamilies[queueFamilyIndex];
            if (queueFamily.queueCount > 0) {
                if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                    graphicsFamilyIndex = queueFamilyIndex;
                VkBool32 supportsSurfacePresentation;
                vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice.handle, queueFamilyIndex, m_SurfaceHandle, &supportsSurfacePresentation);
                if (supportsSurfacePresentation)
                    presentationFamilyIndex = queueFamilyIndex;
            }
            if (graphicsFamilyIndex.has_value() && presentationFamilyIndex.has_value()) {
                hasRequiredQueueFamilies = true;
                break;
            }
        }
        if (!hasRequiredQueueFamilies) {
            const std::string errorMessage = "No collection of queue families found where all requirements are met";
            logging::Log(logging::LogType::ERROR_LOG, errorMessage);
            MessageBox(nullptr, errorMessage.c_str(), m_Title.c_str(), MB_ICONERROR);
        }
        m_QueueFamilyIndices = {graphicsFamilyIndex.value(), presentationFamilyIndex.value()};
        const std::set<uint32> uniqueQueueFamilyIndices = {m_QueueFamilyIndices.graphicsFamilyIndex, m_QueueFamilyIndices.presentationFamilyIndex};
        std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInformation(uniqueQueueFamilyIndices.size());
        {
            const float priority = 1.0f;
            unsigned int index = 0;
            for (uint32 queueFamilyIndex : uniqueQueueFamilyIndices) {
                deviceQueueCreateInformation[index++] = {
                        VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                        nullptr,
                        0,
                        queueFamilyIndex,
                        1,
                        &priority
                };
            }
        }
        VkPhysicalDeviceFeatures physicalDeviceFeatures = {};
        VkDeviceCreateInfo deviceCreateInformation{
                VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                nullptr,
                0,
                static_cast<uint32>(deviceQueueCreateInformation.size()),
                deviceQueueCreateInformation.data(),
#ifdef VALIDATION_LAYERS_ENABLED
                static_cast<uint32>(m_ValidationLayers.size()), m_ValidationLayers.data(),
#else
                0, nullptr,
#endif
                static_cast<uint32_t>(m_RequiredDeviceExtensions.size()), m_RequiredDeviceExtensions.data(),
                &physicalDeviceFeatures
        };
        if (VkResult result = vkCreateDevice(m_PhysicalDevice.handle, &deviceCreateInformation, nullptr, &m_LogicalDeviceHandle); result !=
                                                                                                                                  VK_SUCCESS) {
            const std::string errorMessage = util::Format("Error code %i, could not create logical Vulkan device", MAX_MESSAGE_LENGTH, result);
            logging::Log(logging::LogType::ERROR_LOG, errorMessage);
            MessageBox(nullptr, errorMessage.c_str(), m_Title.c_str(), MB_ICONERROR);
            return;
        }
        logging::Log(logging::LogType::INFORMATION_LOG, "Successfully created logical Vulkan device");
        vkGetDeviceQueue(m_LogicalDeviceHandle, m_QueueFamilyIndices.graphicsFamilyIndex, 0, &m_GraphicsQueueHandle);
    }

    void VulkanWindow::CreateSwapChain() {
        VkSurfaceFormatKHR surfaceFormat;
        if (m_PhysicalDevice.supportedSurfaceFormats.size() > 1) {
            for (const auto& availableFormat : m_PhysicalDevice.supportedSurfaceFormats) {
                if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                    surfaceFormat = availableFormat;
            }
        } else {
            surfaceFormat = m_PhysicalDevice.supportedSurfaceFormats.front();
        }
        VkPresentModeKHR presentationMode;
        bool foundImmediate = false;
        for (const auto& availablePresentationMode : m_PhysicalDevice.supportedPresentationModes) {
            if (availablePresentationMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
                presentationMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
                foundImmediate = true;
            }
        }
        if (!foundImmediate) presentationMode = VK_PRESENT_MODE_FIFO_KHR;
        uint32 numberOfImages = m_PhysicalDevice.surfaceCapabilities.minImageCount + 1;
        if (m_PhysicalDevice.surfaceCapabilities.maxImageCount > 0 && numberOfImages > m_PhysicalDevice.surfaceCapabilities.maxImageCount)
            numberOfImages = m_PhysicalDevice.surfaceCapabilities.maxImageCount;
        const bool sameQueueFamilyIndices = m_QueueFamilyIndices.graphicsFamilyIndex == m_QueueFamilyIndices.presentationFamilyIndex;
        const VkSharingMode sharingMode = sameQueueFamilyIndices
                                          ? VK_SHARING_MODE_EXCLUSIVE
                                          : VK_SHARING_MODE_CONCURRENT;
        const VkExtent2D extent = m_PhysicalDevice.surfaceCapabilities.currentExtent;
        VkSwapchainCreateInfoKHR swapchainCreationInformation{
                VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                nullptr,
                0,
                m_SurfaceHandle,
                numberOfImages,
                surfaceFormat.format, surfaceFormat.colorSpace,
                extent,
                1,
                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                sharingMode,
                0, nullptr,
                m_PhysicalDevice.surfaceCapabilities.currentTransform,
                VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
                presentationMode,
                VK_TRUE,
                VK_NULL_HANDLE
        };
        if (!sameQueueFamilyIndices) {
            swapchainCreationInformation.queueFamilyIndexCount = NUMBER_OF_QUEUE_INDICES;
            const std::array<uint32, NUMBER_OF_QUEUE_INDICES> queueFamilyIndices = {m_QueueFamilyIndices.graphicsFamilyIndex,
                                                                                    m_QueueFamilyIndices.presentationFamilyIndex};
            swapchainCreationInformation.pQueueFamilyIndices = queueFamilyIndices.data();
        }
        if (const VkResult result = vkCreateSwapchainKHR(m_LogicalDeviceHandle, &swapchainCreationInformation, nullptr, &m_SwapchainHandle); result !=
                                                                                                                                             VK_SUCCESS) {
            const std::string errorMessage = util::Format("Error code %i, could not create Vulkan swapchain", MAX_MESSAGE_LENGTH, result);
            logging::Log(logging::LogType::ERROR_LOG, errorMessage);
            MessageBox(nullptr, errorMessage.c_str(), m_Title.c_str(), MB_ICONERROR);
        }
        logging::Log(logging::LogType::INFORMATION_LOG, "Successfully created Vulkan swapchain");
        vkGetSwapchainImagesKHR(m_LogicalDeviceHandle, m_SwapchainHandle, &numberOfImages, nullptr);
        m_SwapchainImageHandles.reserve(numberOfImages);
        vkGetSwapchainImagesKHR(m_LogicalDeviceHandle, m_SwapchainHandle, &numberOfImages, m_SwapchainImageHandles.data());
        m_SwapchainImageFormat = surfaceFormat.format;
        m_SwapchainExtent = extent;
    }

    void VulkanWindow::CreateImageViews() {
        m_SwapchainImageViewHandles.reserve(m_SwapchainImageHandles.size());
        for (int imageIndex = 0; imageIndex < m_SwapchainImageHandles.size(); imageIndex++) {
            VkImageViewCreateInfo imageViewCreateInformation{
                    VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                    nullptr,
                    0,
                    m_SwapchainImageHandles[imageIndex],
                    VK_IMAGE_VIEW_TYPE_2D,
                    m_SwapchainImageFormat,
                    {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY},
                    {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}
            };
            if (const VkResult result = vkCreateImageView(m_LogicalDeviceHandle, &imageViewCreateInformation, nullptr,
                                                          &m_SwapchainImageViewHandles[imageIndex]); result != VK_SUCCESS) {
                const std::string errorMessage = util::Format("Error code %i, could not create image view", MAX_MESSAGE_LENGTH, result);
                logging::Log(logging::LogType::ERROR_LOG, errorMessage);
                MessageBox(nullptr, errorMessage.c_str(), m_Title.c_str(), MB_ICONERROR);
                return;
            }
        }
        logging::Log(logging::LogType::INFORMATION_LOG, "Successfully created Vulkan swapchain image views");
    }

    void VulkanWindow::CreatePipeline() {
        const auto
                vertexShaderSource = file::ReadFile("shaders/vert.spv"),
                fragmentShaderSource = file::ReadFile("shaders/frag.spv");
        m_VertexShaderModuleHandle = CreateShaderModule(vertexShaderSource);
        m_FragmentShaderModuleHandle = CreateShaderModule(fragmentShaderSource);
        VkPipelineShaderStageCreateInfo
                vertexShaderStateCreationInformation{
                VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                nullptr,
                0,
                VK_SHADER_STAGE_VERTEX_BIT,
                m_VertexShaderModuleHandle,
                "main",
                nullptr
        },
                fragmentShaderStateCreationInformation{
                VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                nullptr,
                0,
                VK_SHADER_STAGE_FRAGMENT_BIT,
                m_FragmentShaderModuleHandle,
                "main",
                nullptr
        };
        std::array<VkPipelineShaderStageCreateInfo, 2> shaderStates = {vertexShaderStateCreationInformation, fragmentShaderStateCreationInformation};
        VkPipelineVertexInputStateCreateInfo vertexInputStateCreationInformation{
                VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
                nullptr,
                0,
                0, nullptr,
                0, nullptr
        };
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreationInformation{
                VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
                nullptr,
                0,
                VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                VK_FALSE
        };
        VkViewport viewport = {
                0.0f, 0.0f, static_cast<float>(m_SwapchainExtent.width), static_cast<float>(m_SwapchainExtent.height),
                0.0f, 1.0f
        };
        VkRect2D scissor = {{0, 0}, m_SwapchainExtent};
        VkPipelineViewportStateCreateInfo viewportStateCreationInformation{
                VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
                nullptr,
                0,
                1,
                &viewport,
                1,
                &scissor
        };
        VkPipelineRasterizationStateCreateInfo rasterizationStateCreationInformation{
                VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
                nullptr,
                0,
                VK_FALSE,
                VK_FALSE,
                VK_POLYGON_MODE_FILL,
                VK_CULL_MODE_BACK_BIT,
                VK_FRONT_FACE_CLOCKWISE,
                VK_FALSE,
                0.0f, 0.0f, 0.0f
        };
        VkPipelineMultisampleStateCreateInfo multisampleStateCreationInformation{
                VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
                nullptr,
                0,
                VK_SAMPLE_COUNT_1_BIT,
                VK_FALSE,
                1.0f,
                nullptr,
                VK_FALSE,
                VK_FALSE
        };
        VkPipelineColorBlendAttachmentState colorBlendAttachmentState{
                VK_FALSE,
                VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO,
                VK_BLEND_OP_ADD,
                VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO,
                VK_BLEND_OP_ADD,
                0
        };
        VkPipelineColorBlendStateCreateInfo colorBlendStateCreationInformation{
                VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
                nullptr,
                0,
                VK_FALSE,
                VK_LOGIC_OP_COPY,
                1,
                &colorBlendAttachmentState,
                {0.0f, 0.0f, 0.0f, 0.0f}
        };
        VkPipelineLayoutCreateInfo pipelineLayoutCreationInformation{
                VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
                nullptr,
                0,
                0, nullptr,
                0, nullptr
        };
        if (const VkResult result = vkCreatePipelineLayout(m_LogicalDeviceHandle, &pipelineLayoutCreationInformation, nullptr,
                                                           &m_PipelineLayoutHandle); result != VK_SUCCESS) {
            const std::string errorMessage = util::Format("Error code %i, could not create Vulkan pipeline", MAX_MESSAGE_LENGTH, result);
            logging::Log(logging::LogType::ERROR_LOG, errorMessage);
            MessageBox(nullptr, errorMessage.c_str(), m_Title.c_str(), MB_ICONERROR);
        }
        vkDestroyShaderModule(m_LogicalDeviceHandle, m_VertexShaderModuleHandle, nullptr);
        vkDestroyShaderModule(m_LogicalDeviceHandle, m_FragmentShaderModuleHandle, nullptr);
    }

    void VulkanWindow::CreateRenderPass() {
        VkAttachmentDescription colorAttachment{
                0,
                m_SwapchainImageFormat,
                VK_SAMPLE_COUNT_1_BIT,
                VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
                VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE,
                VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
        };

    }

    VkShaderModule VulkanWindow::CreateShaderModule(const std::vector<char>& shaderSource) {
        VkShaderModuleCreateInfo creationInformation{
                VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                nullptr,
                0,
                shaderSource.size(),
                reinterpret_cast<const uint32*>(shaderSource.data())
        };
        VkShaderModule shaderModule;
        if (vkCreateShaderModule(m_LogicalDeviceHandle, &creationInformation, nullptr, &shaderModule) != VK_SUCCESS) {
            logging::Log(logging::LogType::ERROR_LOG, "Could not compile shader");
            return nullptr;
        }
        return shaderModule;
    }
}
