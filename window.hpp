#pragma once

#include <windows.h>
#include <iostream>

#include "windows_definitions.hpp"
#include "type_definitions.hpp"
#include "logger.hpp"
#include "application.hpp"

namespace voxelfield::window {
    class Window {
    private:
        const char* m_Title;
        WindowClass m_WindowClass;
        WindowHandle m_Handle;
        Application& m_Application;

        static long long
        WindowProcess(WindowHandle windowHandle, unsigned int message, unsigned long long messageParameter, long long longMessageParameter);

    public:
        Window(Application& application, std::string title);

        Window() = delete;

        bool Open();

        void SetFullscreen(bool isFullScreen);
    };
}
