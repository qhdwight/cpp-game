#pragma once

#include <windows.h>
#include <iostream>

#include "windows_definitions.hpp"
#include "type_definitions.hpp"
#include "logger.hpp"
#include "application.hpp"

namespace voxelfield::window {
    class Window {
    public:
        Window(Application& application, const std::string& title);

        Window() = delete;

        virtual ~Window();

        virtual void Open();

        void Loop();

        void SetFullscreen(bool isFullScreen);

    protected:
        std::string m_Title;
        WindowClass m_WindowClass;
        WindowHandle m_Handle;
        Application& m_Application;

        static long long WindowProcess
                (WindowHandle windowHandle, unsigned int message, unsigned long long messageParameter, long long longMessageParameter);

        virtual void Draw() {}
    };
}
