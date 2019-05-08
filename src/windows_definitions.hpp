#pragma once

#include <windows.h>

namespace voxelfield {
    typedef RECT Rectangle;
    typedef DEVMODE DeviceData;
    typedef MSG WindowMessage;
    typedef WNDCLASSEX WindowClass;
    typedef HWND WindowHandle;
    typedef HDC DrawingContextHandle;
    typedef HINSTANCE ApplicationHandle;
    typedef PIXELFORMATDESCRIPTOR PixelFormatDescriptor;
}
