#include "window.hpp"

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
        switch (message) {
            case WM_CREATE: {
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
            }
            case WM_PAINT: {
                ValidateRect(windowHandle, nullptr);
                break;
            }
            case WM_KILLFOCUS: {
                break;
            }
            case WM_SETFOCUS: {
                break;
            }
            case WM_DESTROY: {
                DestroyWindow(windowHandle);
                PostQuitMessage(0);
                break;
            }
            default: {
                return DefWindowProc(windowHandle, message, messageParameter, longMessageParameter);
            }
        }
        return 0;
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
        SetWindowLongPtr(m_Handle, GWLP_USERDATA, reinterpret_cast<long long>(this));
        ShowWindow(m_Handle, SW_SHOW);
        SetForegroundWindow(m_Handle);
        SetFocus(m_Handle);
    }

    void Window::Loop() {
        WindowMessage message;
        while (GetMessage(&message, nullptr, 0, 0)) {
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
            TranslateMessage(&message);
            DispatchMessage(&message);
            Draw();
        }
    }

    void Window::SetFullscreen(bool isFullScreen) {

    }
}