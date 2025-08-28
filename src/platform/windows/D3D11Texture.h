#pragma once

#include "../ITexture.h"

#ifdef PLATFORM_WINDOWS

#include <d3d11.h>
#include <wrl/client.h>

class D3D11Texture : public ITexture
{
public:
    D3D11Texture();
    ~D3D11Texture() override;

    bool Create(const TextureDesc& desc) override;
    bool Update(const void* data, size_t dataSize, size_t rowPitch = 0) override;
    void Destroy() override;

    int GetWidth() const override { return m_width; }
    int GetHeight() const override { return m_height; }
    TextureFormat GetFormat() const override { return m_format; }

    void* GetNativeHandle() const override { return m_texture.Get(); }
    void* GetShaderResourceView() const override { return m_shaderResourceView.Get(); }

    std::string_view GetPlatformName() const noexcept override { return "DirectX 11"; }

    bool IsValid() const override { return m_texture != nullptr; }

    // D3D11 specific methods
    ID3D11Texture2D* GetD3D11Texture() const { return m_texture.Get(); }
    ID3D11ShaderResourceView* GetD3D11SRV() const { return m_shaderResourceView.Get(); }
    
    void SetDevice(ID3D11Device* device);

private:
    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;

    int m_width = 0;
    int m_height = 0;
    TextureFormat m_format = TextureFormat::BGRA8;
    TextureUsage m_usage = TextureUsage::Dynamic;

    DXGI_FORMAT GetDXGIFormat(TextureFormat format) const;
    D3D11_USAGE GetD3D11Usage(TextureUsage usage) const;
    UINT GetD3D11BindFlags(TextureUsage usage) const;
    UINT GetD3D11CPUAccessFlags(TextureUsage usage) const;
};

#endif // PLATFORM_WINDOWS