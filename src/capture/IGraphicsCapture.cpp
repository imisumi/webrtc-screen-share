#include "IGraphicsCapture.h"

#ifdef PLATFORM_WINDOWS
#include "windows/WindowsGraphicsCapture.h"
#endif

#ifdef PLATFORM_MACOS
#include "macos/MacOSGraphicsCapture.h"
#endif

#ifdef PLATFORM_LINUX
#include "linux/LinuxGraphicsCapture.h"
#endif

std::unique_ptr<IGraphicsCapture> IGraphicsCapture::Create()
{
#ifdef PLATFORM_WINDOWS
    return std::make_unique<WindowsGraphicsCapture>();
#elif PLATFORM_MACOS
    return std::make_unique<MacOSGraphicsCapture>();
#elif PLATFORM_LINUX
    return std::make_unique<LinuxGraphicsCapture>();
#else
    static_assert(false, "Unsupported platform for Graphics Capture");
    return nullptr;
#endif
}

std::string_view IGraphicsCapture::GetCurrentPlatform() noexcept
{
#ifdef PLATFORM_WINDOWS
    return "Windows";
#elif PLATFORM_MACOS
    return "macOS";
#elif PLATFORM_LINUX
    return "Linux";
#else
    return "Unknown";
#endif
}