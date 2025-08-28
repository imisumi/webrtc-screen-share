#include "D3D11Renderer.h"
#include "../IWindow.h"
#include <iostream>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

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
		std::cout << "Invalid window provided to D3D11Renderer" << std::endl;
		return false;
	}

	m_window = window;

	// Get window size and handle
	int width, height;
	window->GetSize(width, height);
	HWND hwnd = static_cast<HWND>(window->GetNativeHandle());

	if (!CreateDeviceAndSwapChain(hwnd, width, height))
	{
		std::cout << "Failed to create D3D11 device and swap chain" << std::endl;
		return false;
	}

	CreateRenderTarget();

	// Initialize ImGui
	if (!InitializeImGui())
	{
		std::cout << "Failed to initialize ImGui for D3D11" << std::endl;
		return false;
	}

	std::cout << "D3D11Renderer initialized successfully" << std::endl;
	return true;
}

void D3D11Renderer::Shutdown()
{
	ShutdownImGui();
	CleanupRenderTarget();

	m_swapChain.Reset();
	m_context.Reset();
	m_device.Reset();
	m_window = nullptr;
}

void D3D11Renderer::BeginFrame()
{
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
			std::cout << "Failed to resize swap chain buffers" << std::endl;
		}
	}

	// Check for swap chain occlusion
	if (m_swapChainOccluded && m_swapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
	{
		Sleep(10);
		return;
	}
	m_swapChainOccluded = false;

	// Begin ImGui frame
	NewImGuiFrame();
}

void D3D11Renderer::EndFrame()
{
	// Just render ImGui, don't present yet
	RenderImGui();
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
	if (m_context && m_renderTargetView)
	{
		const float clearColor[4] = {r, g, b, a};
		m_context->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);
	}
}

void D3D11Renderer::SetViewport(int x, int y, int width, int height)
{
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

bool D3D11Renderer::InitializeImGui()
{
	if (!m_device || !m_context)
		return false;

	HWND hwnd = static_cast<HWND>(m_window->GetNativeHandle());

	// Check if ImGui is already initialized
	ImGuiContext *context = ImGui::GetCurrentContext();
	if (!context)
	{
		// Setup ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO &io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		// Setup ImGui style
		ImGui::StyleColorsDark();

		// When viewports are enabled we tweak WindowRounding/WindowBg
		// so platform windows can look identical to regular ones
		ImGuiStyle &style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}
	}

	// Check if backends are already initialized
	ImGuiIO &io = ImGui::GetIO();
	if (!io.BackendPlatformUserData)
	{
		ImGui_ImplWin32_Init(hwnd);
	}

	if (!io.BackendRendererUserData)
	{
		ImGui_ImplDX11_Init(m_device.Get(), m_context.Get());
	}

	return true;
}

void D3D11Renderer::ShutdownImGui()
{
	//TODO: fix maybe
	// ImGuiIO& io = ImGui::GetIO();

	// if (io.BackendRendererUserData)
	// ImGui_ImplDX11_Shutdown();

	// // if (io.BackendPlatformUserData)
	// ImGui_ImplWin32_Shutdown();

	// if (ImGui::GetCurrentContext())
	// 	ImGui::DestroyContext();

	// ImGui_ImplDX11_Shutdown();
	// ImGui_ImplWin32_Shutdown();
	// ImGui::DestroyContext();
}

void D3D11Renderer::NewImGuiFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// Enable docking over the main viewport
	ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
}

void D3D11Renderer::RenderImGui()
{
	ImGuiIO &io = ImGui::GetIO();

	// Rendering
	ImGui::Render();

	// Set render target
	m_context->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), nullptr);

	// Render ImGui
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
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
		std::cout << "D3D11CreateDeviceAndSwapChain failed with HRESULT: 0x" << std::hex << res << std::endl;
		return false;
	}

	return true;
}

void D3D11Renderer::CreateRenderTarget()
{
	ComPtr<ID3D11Texture2D> backBuffer;
	m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
	if (backBuffer)
	{
		m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.GetAddressOf());
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