#include "ITexture.h"

#include <memory>

#ifdef PLATFORM_WINDOWS
#include "windows/D3D11Texture.h"
#endif

std::unique_ptr<ITexture> ITexture::Create()
{
#ifdef PLATFORM_WINDOWS
	return std::make_unique<D3D11Texture>();
#elif defined(PLATFORM_MACOS)
	// return std::make_unique<MetalTexture>();
	static_assert(false, "macOS texture implementation not yet available");
	return nullptr;
#elif defined(PLATFORM_LINUX)
	// return std::make_unique<OpenGLTexture>();
	static_assert(false, "Linux texture implementation not yet available");
	return nullptr;
#else
	static_assert(false, "No texture implementation for this platform");
	return nullptr;
#endif
}

std::string_view ITexture::GetTextureFormatName(TextureFormat format) noexcept
{
	switch (format)
	{
	case TextureFormat::BGRA8:
		return "BGRA8";
	case TextureFormat::RGBA8:
		return "RGBA8";
	case TextureFormat::RGB8:
		return "RGB8";
	case TextureFormat::R8:
		return "R8";
	default:
		return "Unknown";
	}
}

std::string_view ITexture::GetTextureUsageName(TextureUsage usage) noexcept
{
	switch (usage)
	{
	case TextureUsage::Dynamic:
		return "Dynamic";
	case TextureUsage::Static:
		return "Static";
	case TextureUsage::RenderTarget:
		return "RenderTarget";
	default:
		return "Unknown";
	}
}
