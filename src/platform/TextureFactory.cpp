#include "TextureFactory.h"

#ifdef PLATFORM_WINDOWS
#include "windows/D3D11Texture.h"
#endif

std::unique_ptr<ITexture> TextureFactory::Create(RendererAPI api)
{
    if (api == RendererAPI::Auto)
    {
        api = WindowFactory::GetBestRendererAPI();
    }

    switch (api)
    {
#ifdef PLATFORM_WINDOWS
    case RendererAPI::DirectX11:
    case RendererAPI::DirectX12: // D3D12 can fall back to D3D11 textures for now
        return std::make_unique<D3D11Texture>();
#endif
    
    case RendererAPI::OpenGL:
        // TODO: Implement OpenGLTexture
        return nullptr;
        
    case RendererAPI::Vulkan:
        // TODO: Implement VulkanTexture
        return nullptr;
        
    case RendererAPI::Metal:
        // TODO: Implement MetalTexture
        return nullptr;
        
    default:
        return nullptr;
    }
}

std::string_view TextureFactory::GetTextureFormatName(TextureFormat format) noexcept
{
    switch (format)
    {
    case TextureFormat::BGRA8: return "BGRA8";
    case TextureFormat::RGBA8: return "RGBA8";
    case TextureFormat::RGB8:  return "RGB8";
    case TextureFormat::R8:    return "R8";
    default:                   return "Unknown";
    }
}

std::string_view TextureFactory::GetTextureUsageName(TextureUsage usage) noexcept
{
    switch (usage)
    {
    case TextureUsage::Dynamic:      return "Dynamic";
    case TextureUsage::Static:       return "Static";
    case TextureUsage::RenderTarget: return "RenderTarget";
    default:                         return "Unknown";
    }
}