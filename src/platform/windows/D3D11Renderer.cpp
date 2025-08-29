#include "D3D11Renderer.h"
#include "../IWindow.h"
#include "../Logger.h"
#include <chrono>
#include <sstream>

D3D11Renderer::D3D11Renderer()
{
}

D3D11Renderer::~D3D11Renderer()
{
	Shutdown();
}

bool D3D11Renderer::Initialize(IWindow *window)
{
	if (!window)
	{
		Logger::Error("Invalid window provided to D3D11Renderer");
		return false;
	}

	m_window = window;

	// Get window size and handle
	int width, height;
	window->GetSize(width, height);
	HWND hwnd = static_cast<HWND>(window->GetNativeHandle());

	if (!CreateDeviceAndSwapChain(hwnd, width, height))
	{
		Logger::Error("Failed to create D3D11 device and swap chain");
		return false;
	}

	CreateRenderTarget();

	Logger::Info("D3D11Renderer initialized successfully");
	return true;
}

void D3D11Renderer::Shutdown()
{
	CleanupRenderTarget();

	m_swapChain.Reset();
	m_context.Reset();
	m_device.Reset();
	m_window = nullptr;
}

void D3D11Renderer::BeginFrame()
{
	m_frameStartTime = std::chrono::high_resolution_clock::now();

	// Handle any pending resize
	if (m_resizeWidth != 0 && m_resizeHeight != 0)
	{
		CleanupRenderTarget();
		HRESULT hr = m_swapChain->ResizeBuffers(0, m_resizeWidth, m_resizeHeight, DXGI_FORMAT_UNKNOWN, 0);
		if (SUCCEEDED(hr))
		{
			m_resizeWidth = m_resizeHeight = 0;
			CreateRenderTarget();
		}
		else
		{
			Logger::Error("Failed to resize swap chain buffers");
		}
	}

	// Check for swap chain occlusion
	if (m_swapChainOccluded && m_swapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
	{
		Sleep(10);
		return;
	}
	m_swapChainOccluded = false;

	m_context->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), nullptr);
}

void D3D11Renderer::EndFrame()
{
	auto frameEndTime = std::chrono::high_resolution_clock::now();
	auto frameDuration = std::chrono::duration_cast<std::chrono::microseconds>(frameEndTime - m_frameStartTime);

	m_stats.frameCount++;
	m_stats.frameTimeMs = frameDuration.count() / 1000.0f;

	// Update FPS every 60 frames
	if (m_stats.frameCount % 60 == 0)
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		if (m_lastFpsUpdate.time_since_epoch().count() > 0)
		{
			auto timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - m_lastFpsUpdate);
			if (timeDiff.count() > 0)
			{
				m_stats.fps = 60000.0f / timeDiff.count();
			}
		}
		m_lastFpsUpdate = currentTime;
	}
}

void D3D11Renderer::Present()
{
	// Present the frame
	HRESULT hr = m_swapChain->Present(m_vsync ? 1 : 0, 0);
	m_swapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
}

void D3D11Renderer::OnWindowResize(int width, int height)
{
	// Queue resize for next frame
	if (width > 0 && height > 0)
	{
		m_resizeWidth = width;
		m_resizeHeight = height;
	}
}

void D3D11Renderer::Clear(float r, float g, float b, float a)
{
	if (r < 0.0f || r > 1.0f || g < 0.0f || g > 1.0f || b < 0.0f || b > 1.0f || a < 0.0f || a > 1.0f)
	{
		Logger::Warning("Invalid clear color values. All components must be in range [0.0, 1.0]");
		return;
	}

	if (m_context && m_renderTargetView)
	{
		const float clearColor[4] = {r, g, b, a};
		m_context->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);
		m_stats.clearCalls++;
	}
}

void D3D11Renderer::SetViewport(int x, int y, int width, int height)
{
	if (width <= 0 || height <= 0)
	{
		Logger::Warning("Invalid viewport dimensions. Width and height must be positive");
		return;
	}

	if (m_context)
	{
		D3D11_VIEWPORT viewport = {};
		viewport.TopLeftX = static_cast<float>(x);
		viewport.TopLeftY = static_cast<float>(y);
		viewport.Width = static_cast<float>(width);
		viewport.Height = static_cast<float>(height);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		m_context->RSSetViewports(1, &viewport);
	}
}

RenderStats D3D11Renderer::GetStats() const
{
	return m_stats;
}

bool D3D11Renderer::CreateDeviceAndSwapChain(HWND hwnd, int width, int height)
{
	// Swap chain description
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = 2;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hwnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = {D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0};

	HRESULT res = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		createDeviceFlags,
		featureLevelArray,
		2,
		D3D11_SDK_VERSION,
		&sd,
		m_swapChain.GetAddressOf(),
		m_device.GetAddressOf(),
		&featureLevel,
		m_context.GetAddressOf());

	if (FAILED(res))
	{
		std::ostringstream oss;
		oss << "D3D11CreateDeviceAndSwapChain failed with HRESULT: 0x" << std::hex << res;
		Logger::Error(oss.str());
		return false;
	}

	return true;
}

void D3D11Renderer::CreateRenderTarget()
{
	ComPtr<ID3D11Texture2D> backBuffer;
	HRESULT hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
	if (SUCCEEDED(hr) && backBuffer)
	{
		hr = m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.GetAddressOf());
		if (FAILED(hr))
		{
			std::ostringstream oss;
			oss << "Failed to create render target view with HRESULT: 0x" << std::hex << hr;
			Logger::Error(oss.str());
		}
	}
	else
	{
		std::ostringstream oss;
		oss << "Failed to get back buffer with HRESULT: 0x" << std::hex << hr;
		Logger::Error(oss.str());
	}
}

void D3D11Renderer::CleanupRenderTarget()
{
	m_renderTargetView.Reset();
}

void D3D11Renderer::HandleDeviceLost()
{
	// TODO: Handle D3D11 device lost scenarios
}
