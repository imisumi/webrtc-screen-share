#pragma once

#include <memory>
#include <string_view>

enum class TextureFormat
{
	BGRA8, // 32-bit BGRA (most common for screen capture)
	RGBA8, // 32-bit RGBA
	RGB8,  // 24-bit RGB
	R8	   // 8-bit single channel
};

enum class TextureUsage
{
	Dynamic,	 // CPU write, GPU read (for updating capture data)
	Static,		 // GPU only (for static textures)
	RenderTarget // GPU render target
};

struct TextureDesc
{
	int width = 0;
	int height = 0;
	TextureFormat format = TextureFormat::BGRA8;
	TextureUsage usage = TextureUsage::Dynamic;
	bool generateMips = false;
};

class ITexture
{
public:
	virtual ~ITexture() = default;

	virtual bool Create(const TextureDesc &desc) = 0;
	virtual bool Update(const void *data, size_t dataSize, size_t rowPitch = 0) = 0;
	virtual void Destroy() = 0;

	virtual int GetWidth() const = 0;
	virtual int GetHeight() const = 0;
	virtual TextureFormat GetFormat() const = 0;

	virtual void *GetNativeHandle() const = 0;
	virtual void *GetShaderResourceView() const = 0;

	virtual std::string_view GetPlatformName() const noexcept = 0;

	virtual bool IsValid() const = 0;

	static std::unique_ptr<ITexture> Create();
	static std::string_view GetTextureFormatName(TextureFormat format) noexcept;
	static std::string_view GetTextureUsageName(TextureUsage usage) noexcept;
};