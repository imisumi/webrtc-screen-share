#pragma once

#include "../IRenderer.h"
#include <d3d11.h>
#include <dxgi.h>
#include <wrl/client.h>
#include <chrono>

using Microsoft::WRL::ComPtr;

class IWindow;

class D3D11Renderer : public IRenderer
{
public:
	D3D11Renderer();
	~D3D11Renderer() override;

	bool Initialize(IWindow *window) override;
	void Shutdown() override;

	void BeginFrame() override;
	void EndFrame() override;
	void Present() override;

	void Clear(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f) override;
	void SetViewport(int x, int y, int width, int height) override;

	// Handle window resize
	void OnWindowResize(int width, int height) override;

	RenderStats GetStats() const override;
	std::string_view GetRendererName() const noexcept override { return "DirectX 11"; }
	void *GetDevice() const override { return m_device.Get(); }
	void *GetDeviceContext() const override { return m_context.Get(); }

	// D3D11 specific
	ID3D11Device *GetD3D11Device() const { return m_device.Get(); }
	ID3D11DeviceContext *GetD3D11Context() const { return m_context.Get(); }
	IDXGISwapChain *GetSwapChain() const { return m_swapChain.Get(); }

private:
	bool CreateDeviceAndSwapChain(HWND hwnd, int width, int height);
	void CreateRenderTarget();
	void CleanupRenderTarget();
	void HandleDeviceLost();

private:
	IWindow *m_window = nullptr;

	ComPtr<ID3D11Device> m_device;
	ComPtr<ID3D11DeviceContext> m_context;
	ComPtr<IDXGISwapChain> m_swapChain;
	ComPtr<ID3D11RenderTargetView> m_renderTargetView;

	bool m_vsync = true;
	bool m_swapChainOccluded = false;
	RenderStats m_stats;

	// Resize handling
	int m_resizeWidth = 0;
	int m_resizeHeight = 0;

	// Timing for stats
	std::chrono::high_resolution_clock::time_point m_frameStartTime;
	std::chrono::high_resolution_clock::time_point m_lastFpsUpdate;
};