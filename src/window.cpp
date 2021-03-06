#include "window.hpp"

#include <chrono>

namespace voxelfield::window {
    Window::Window(Application& application, const std::string& title) : m_Application(application) {
        m_Title = title;
        m_WindowClass = {
                sizeof(WindowClass),
                CS_OWNDC,
                WindowProcess,
                0, 0,
                application.GetHandle(),
                nullptr,
                LoadCursor(nullptr, IDC_ARROW),
                (HBRUSH) COLOR_BACKGROUND,
                nullptr,
                application.GetName().c_str(),
                nullptr
        };
    }

    Window::~Window() {
        DestroyWindow(m_Handle);
    }

    long long
    Window::WindowProcess(WindowHandle windowHandle, unsigned int message, unsigned long long messageParameter, long long longMessageParameter) {
//        auto* meme = reinterpret_cast<Window*>(GetWindowLongPtr(windowHandle, GWLP_USERDATA));
//        logging::Log(logging::LogType::INFORMATION_LOG, meme->m_Title);
        return DefWindowProc(windowHandle, message, messageParameter, longMessageParameter);
    }

    void Window::Open() {
        if (!RegisterClassEx(&m_WindowClass)) {
            const std::string errorMessage = "Failed to register window class! This should not ever happen... Ever. So I'm not sure what to say.";
            throw std::runtime_error(errorMessage);
        }
        logging::Log(logging::LogType::INFORMATION_LOG, "Successfully registered window class");
        m_Handle = CreateWindow(
                m_Application.GetName().c_str(),
                m_Title.c_str(),
                WS_OVERLAPPEDWINDOW, 0, 0, 640, 480,
                nullptr,
                nullptr,
                m_Application.GetHandle(),
                nullptr
        );
        if (!m_Handle) {
            const std::string errorMessage = "Failed to create the window! This should not ever happen... Ever. So I'm not sure what to say.";
            throw std::runtime_error(errorMessage);
        }
        logging::Log(logging::LogType::INFORMATION_LOG, "Successfully created the window");
        DrawingContextHandle drawingHandle = GetDC(m_Handle);
        PixelFormatDescriptor pixelFormatDescriptor{
                sizeof(PixelFormatDescriptor),
                1,
                PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
                PFD_TYPE_RGBA,
                32,                 // Color depth
                0, 0, 0, 0, 0, 0,
                24,                 // Number of bits in depth buffer
                8,                  // Number of bits in stencil buffer
                0,                  // Number of auxiliary buffers in frame buffer
                PFD_MAIN_PLANE,
                0,
                0, 0, 0
        };
        int pixelFormat = ChoosePixelFormat(drawingHandle, &pixelFormatDescriptor);
        if (!pixelFormat) {
            throw std::runtime_error("Could not find a suitable pixel format");
        }
        if (!SetPixelFormat(drawingHandle, pixelFormat, &pixelFormatDescriptor)) {
            throw std::runtime_error("Could not set pixel format");
        }
        DescribePixelFormat(drawingHandle, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pixelFormatDescriptor);
        ReleaseDC(m_Handle, drawingHandle);
        SetWindowLongPtr(m_Handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
        ShowWindow(m_Handle, SW_SHOW);
        SetForegroundWindow(m_Handle);
        SetFocus(m_Handle);
    }

    void Window::Loop() {
        WindowMessage message;
        while (GetMessage(&message, m_Handle, 0, 0)) {
            auto start = std::chrono::high_resolution_clock::now();
            Draw();
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double, std::ratio<1, 1>>(end - start);
            logging::Log(logging::LogType::INFORMATION_LOG, util::Format("Time: %f", MAX_MESSAGE_LENGTH, 1.0 / duration.count()));
            TranslateMessage(&message);
            DispatchMessage(&message);
            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
                DestroyWindow(m_Handle);
            }
//            if (GetKeyState(0x46) & 0x8000) {
//                if (s_FKeyStatus == 0) {
//                    s_FKeyStatus = 1;
//                    SetFullScreen(windowHandle, s_Rect, !s_IsFullScreen);
//                } else if (s_FKeyStatus == 1) {
//                    s_FKeyStatus = 2;
//                }
//            } else {
//                s_FKeyStatus = 0;
//            }
        }
    }

    void Window::SetFullscreen(bool isFullScreen) {

    }
}