#pragma once

#include <imgui.h>
#include <stdint.h>

#include <d3d11.h>
#include "capture/GraphicsCaptureFactory.h"

struct GLFWwindow;

class ID3D11Device;
// class ID3D11DeviceContext;
// class IDXGISwapChain;
// class ID3D11RenderTargetView;

class App
{
public:
	App();
	~App();

	void run();

	uint32_t getWidth() const { return m_width; }
	uint32_t getHeight() const { return m_height; }

	void setWidth(uint32_t width) { m_width = width; }
	void setHeight(uint32_t height) { m_height = height; }

private:
	void OnFrameArrived(const FrameData& frame);
	void CreateCaptureTexture(int width, int height);

private:
	bool CreateDeviceD3D(HWND hWnd);

	void CleanupDeviceD3D();
	void CreateRenderTarget();
	void CleanupRenderTarget();
	static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	static App *s_Instance;

	ID3D11Device *g_pd3dDevice = nullptr;
	ID3D11DeviceContext *g_pd3dDeviceContext = nullptr;
	IDXGISwapChain *g_pSwapChain = nullptr;
	bool g_SwapChainOccluded = false;
	uint32_t g_ResizeWidth = 0, g_ResizeHeight = 0;
	ID3D11RenderTargetView *g_mainRenderTargetView = nullptr;

	HWND m_hwnd = nullptr;
	WNDCLASSEXW m_wc = {};

	bool m_show_demo_window = true;
	bool m_show_another_window = false;
	bool m_show_mirror_window = false;
	ImVec4 m_clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	uint32_t m_width = 1920;
	uint32_t m_height = 1080;

	std::unique_ptr<IGraphicsCapture> m_graphicsCapture;
	
	// Capture selection state
	std::string m_selectedSourceId;
	std::string m_selectedSourceName;
	bool m_selectedIsMonitor = false;
	
	// Capture rendering
	ID3D11Texture2D* m_captureTexture = nullptr;
	ID3D11ShaderResourceView* m_captureSRV = nullptr;
	int m_captureWidth = 0;
	int m_captureHeight = 0;
};