#include "vulkan_window.hpp"

voxelfield::window::VulkanWindow::VulkanWindow(voxelfield::Application& application, const std::string& title) : Window(application, title) {
    VkApplicationInfo applicationInformation = {
            VK_STRUCTURE_TYPE_APPLICATION_INFO,
            nullptr,
            application.GetName().c_str(),
            VK_MAKE_VERSION(1, 0, 0),
            ENGINE_NAME,
            VK_MAKE_VERSION(1, 0, 0),
            VK_API_VERSION_1_1
    };
    const char* extensions[] = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME };
    VkInstanceCreateInfo instanceCreationInformation = {
        VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        nullptr,
        0,
        &applicationInformation,
        0, nullptr,
        2, extensions
    };
    VkResult result = vkCreateInstance(&instanceCreationInformation, nullptr, &m_VulkanInstance);
}
