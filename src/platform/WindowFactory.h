#pragma once

#include "IWindow.h"
#include "IRenderer.h"
#include <memory>
#include <string_view>

enum class WindowAPI
{
    Auto,    // Choose best for platform
    Win32,   // Windows native
    Cocoa,   // macOS native  
    X11,     // Linux X11
    Wayland  // Linux Wayland
};

enum class RendererAPI
{
    Auto,        // Choose best for platform
    DirectX11,   // Windows D3D11
    DirectX12,   // Windows D3D12
    OpenGL,      // Cross-platform OpenGL
    Vulkan,      // Cross-platform Vulkan
    Metal        // macOS/iOS Metal
};

class WindowFactory
{
public:
    static std::unique_ptr<IWindow> Create(WindowAPI api = WindowAPI::Auto);
    static std::unique_ptr<IRenderer> CreateRenderer(RendererAPI api = RendererAPI::Auto);
    
    static WindowAPI GetBestWindowAPI();
    static RendererAPI GetBestRendererAPI();
    
    static constexpr std::string_view GetWindowAPIName(WindowAPI api) noexcept;
    static constexpr std::string_view GetRendererAPIName(RendererAPI api) noexcept;

private:
    WindowFactory() = default;
};