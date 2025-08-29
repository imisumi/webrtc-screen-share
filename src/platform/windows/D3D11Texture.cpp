#include "D3D11Texture.h"

#ifdef PLATFORM_WINDOWS

#define NOMINMAX // Prevent Windows from defining min/max macros
#include <iostream>
#include <format>
#include <stdexcept>
#include <algorithm>
#include <cstring>

D3D11Texture::D3D11Texture() = default;

D3D11Texture::~D3D11Texture()
{
	Destroy();
}

bool D3D11Texture::Create(const TextureDesc &desc)
{
	if (desc.width <= 0 || desc.height <= 0)
	{
		std::cout << "Invalid texture dimensions\n";
		return false;
	}

	// Get D3D11 device - this should be injected through constructor in production
	// For now, we'll return false and require external device injection
	if (!m_device)
	{
		std::cout << "No D3D11 device set. Use SetDevice() first.\n";
		return false;
	}

	m_device->GetImmediateContext(&m_context);

	// Store properties
	m_width = desc.width;
	m_height = desc.height;
	m_format = desc.format;
	m_usage = desc.usage;

	// Create D3D11 texture
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = desc.width;
	textureDesc.Height = desc.height;
	textureDesc.MipLevels = desc.generateMips ? 0 : 1; // 0 = generate full mip chain
	textureDesc.ArraySize = 1;
	textureDesc.Format = GetDXGIFormat(desc.format);
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = GetD3D11Usage(desc.usage);
	textureDesc.BindFlags = GetD3D11BindFlags(desc.usage);
	textureDesc.CPUAccessFlags = GetD3D11CPUAccessFlags(desc.usage);
	textureDesc.MiscFlags = desc.generateMips ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

	HRESULT hr = m_device->CreateTexture2D(&textureDesc, nullptr, &m_texture);
	if (FAILED(hr))
	{
		std::cout << std::format("Failed to create D3D11 texture: 0x{:x}\n", hr);
		return false;
	}

	// Create shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = textureDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	hr = m_device->CreateShaderResourceView(m_texture.Get(), &srvDesc, &m_shaderResourceView);
	if (FAILED(hr))
	{
		std::cout << std::format("Failed to create D3D11 shader resource view: 0x{:x}\n", hr);
		m_texture.Reset();
		return false;
	}

	std::cout << std::format("Created D3D11 texture: {}x{}, format: {}, usage: {}\n",
							 desc.width, desc.height,
							 ITexture::GetTextureFormatName(desc.format),
							 ITexture::GetTextureUsageName(desc.usage));

	return true;
}

bool D3D11Texture::Update(const void *data, size_t dataSize, size_t rowPitch)
{
	if (!IsValid() || !data)
	{
		return false;
	}

	if (m_usage != TextureUsage::Dynamic)
	{
		std::cout << "Cannot update non-dynamic texture\n";
		return false;
	}

	// Calculate row pitch if not provided
	size_t bytesPerPixel = 4; // Default for BGRA8
	switch (m_format)
	{
	case TextureFormat::BGRA8:
	case TextureFormat::RGBA8:
		bytesPerPixel = 4;
		break;
	case TextureFormat::RGB8:
		bytesPerPixel = 3;
		break;
	case TextureFormat::R8:
		bytesPerPixel = 1;
		break;
	}

	if (rowPitch == 0)
	{
		rowPitch = m_width * bytesPerPixel;
	}

	// Validate data size
	size_t expectedSize = rowPitch * m_height;
	if (dataSize < expectedSize)
	{
		std::cout << std::format("Data size too small: got {}, expected {}\n", dataSize, expectedSize);
		return false;
	}

	// Map texture and copy data
	D3D11_MAPPED_SUBRESOURCE mapped;
	HRESULT hr = m_context->Map(m_texture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	if (FAILED(hr))
	{
		std::cout << std::format("Failed to map D3D11 texture: 0x{:x}\n", hr);
		return false;
	}

	// Copy row by row to handle different row pitches
	const uint8_t *srcData = static_cast<const uint8_t *>(data);
	uint8_t *dstData = static_cast<uint8_t *>(mapped.pData);

	for (int y = 0; y < m_height; ++y)
	{
		size_t bytesToCopy = (rowPitch < static_cast<size_t>(mapped.RowPitch)) ? rowPitch : static_cast<size_t>(mapped.RowPitch);
		memcpy(dstData + y * mapped.RowPitch,
			   srcData + y * rowPitch,
			   bytesToCopy);
	}

	m_context->Unmap(m_texture.Get(), 0);
	return true;
}

void D3D11Texture::Destroy()
{
	m_shaderResourceView.Reset();
	m_texture.Reset();
	m_context.Reset();
	m_device.Reset();

	m_width = 0;
	m_height = 0;
}

DXGI_FORMAT D3D11Texture::GetDXGIFormat(TextureFormat format) const
{
	switch (format)
	{
	case TextureFormat::BGRA8:
		return DXGI_FORMAT_B8G8R8A8_UNORM;
	case TextureFormat::RGBA8:
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	case TextureFormat::RGB8:
		return DXGI_FORMAT_R8G8B8A8_UNORM; // No direct RGB8 support
	case TextureFormat::R8:
		return DXGI_FORMAT_R8_UNORM;
	default:
		return DXGI_FORMAT_B8G8R8A8_UNORM;
	}
}

D3D11_USAGE D3D11Texture::GetD3D11Usage(TextureUsage usage) const
{
	switch (usage)
	{
	case TextureUsage::Dynamic:
		return D3D11_USAGE_DYNAMIC;
	case TextureUsage::Static:
		return D3D11_USAGE_DEFAULT;
	case TextureUsage::RenderTarget:
		return D3D11_USAGE_DEFAULT;
	default:
		return D3D11_USAGE_DYNAMIC;
	}
}

UINT D3D11Texture::GetD3D11BindFlags(TextureUsage usage) const
{
	UINT flags = D3D11_BIND_SHADER_RESOURCE;

	if (usage == TextureUsage::RenderTarget)
	{
		flags |= D3D11_BIND_RENDER_TARGET;
	}

	return flags;
}

UINT D3D11Texture::GetD3D11CPUAccessFlags(TextureUsage usage) const
{
	switch (usage)
	{
	case TextureUsage::Dynamic:
		return D3D11_CPU_ACCESS_WRITE;
	case TextureUsage::Static:
		return 0;
	case TextureUsage::RenderTarget:
		return 0;
	default:
		return D3D11_CPU_ACCESS_WRITE;
	}
}

void D3D11Texture::SetDevice(ID3D11Device *device)
{
	m_device = device;
}

#endif // PLATFORM_WINDOWS