#pragma once

#include "ITexture.h"
#include "WindowFactory.h" // For RendererAPI enum
#include <memory>
#include <string_view>

class TextureFactory
{
public:
    static std::unique_ptr<ITexture> Create(RendererAPI api = RendererAPI::Auto);
    
    static std::string_view GetTextureFormatName(TextureFormat format) noexcept;
    static std::string_view GetTextureUsageName(TextureUsage usage) noexcept;

private:
    TextureFactory() = default;
};