#include "WindowFactory.h"

#ifdef PLATFORM_WINDOWS
#include "windows/Win32Window.h"
#include "windows/D3D11Renderer.h"
#endif

#include <iostream>

#ifdef PLATFORM_MACOS
// #include "macos/CocoaWindow.h"
// #include "macos/MetalRenderer.h"
#endif

#ifdef PLATFORM_LINUX
// #include "linux/X11Window.h"
// #include "linux/OpenGLRenderer.h"
#endif

std::unique_ptr<IWindow> WindowFactory::Create(WindowAPI api)
{
    if (api == WindowAPI::Auto)
        api = GetBestWindowAPI();

    switch (api)
    {
#ifdef PLATFORM_WINDOWS
    case WindowAPI::Win32:
        return std::make_unique<Win32Window>();
#endif

#ifdef PLATFORM_MACOS
    // case WindowAPI::Cocoa:
    //     return std::make_unique<CocoaWindow>();
#endif

#ifdef PLATFORM_LINUX
    // case WindowAPI::X11:
    //     return std::make_unique<X11Window>();
    // case WindowAPI::Wayland:
    //     return std::make_unique<WaylandWindow>();
#endif

    default:
        return nullptr;
    }
}

std::unique_ptr<IRenderer> WindowFactory::CreateRenderer(RendererAPI api)
{
    if (api == RendererAPI::Auto)
        api = GetBestRendererAPI();

    switch (api)
    {
#ifdef PLATFORM_WINDOWS
    case RendererAPI::DirectX11:
        return std::make_unique<D3D11Renderer>();
    // case RendererAPI::DirectX12:
    //     return std::make_unique<D3D12Renderer>();
#endif

#ifdef PLATFORM_MACOS
    // case RendererAPI::Metal:
    //     return std::make_unique<MetalRenderer>();
#endif

#if defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
    // case RendererAPI::OpenGL:
    //     return std::make_unique<OpenGLRenderer>();
#endif

    // case RendererAPI::Vulkan:  // Cross-platform
    //     return std::make_unique<VulkanRenderer>();

    default:
        return nullptr;
    }
}

WindowAPI WindowFactory::GetBestWindowAPI()
{
#ifdef PLATFORM_WINDOWS
    return WindowAPI::Win32;
#elif defined(PLATFORM_MACOS)
    return WindowAPI::Cocoa;
#elif defined(PLATFORM_LINUX)
    return WindowAPI::X11;  // Prefer X11 over Wayland for now
#else
    return WindowAPI::Auto;
#endif
}

RendererAPI WindowFactory::GetBestRendererAPI()
{
#ifdef PLATFORM_WINDOWS
    return RendererAPI::DirectX11;  // Most compatible on Windows
#elif defined(PLATFORM_MACOS)
    return RendererAPI::Metal;      // Native on macOS
#elif defined(PLATFORM_LINUX)
    return RendererAPI::OpenGL;     // Most compatible on Linux
#else
    return RendererAPI::Auto;
#endif
}

constexpr std::string_view WindowFactory::GetWindowAPIName(WindowAPI api) noexcept
{
    switch (api)
    {
    case WindowAPI::Auto:    return "Auto";
    case WindowAPI::Win32:   return "Win32";
    case WindowAPI::Cocoa:   return "Cocoa";
    case WindowAPI::X11:     return "X11";
    case WindowAPI::Wayland: return "Wayland";
    default:                 return "Unknown";
    }
}

constexpr std::string_view WindowFactory::GetRendererAPIName(RendererAPI api) noexcept
{
    switch (api)
    {
    case RendererAPI::Auto:       return "Auto";
    case RendererAPI::DirectX11:  return "DirectX 11";
    case RendererAPI::DirectX12:  return "DirectX 12";
    case RendererAPI::OpenGL:     return "OpenGL";
    case RendererAPI::Vulkan:     return "Vulkan";
    case RendererAPI::Metal:      return "Metal";
    default:                      return "Unknown";
    }
}