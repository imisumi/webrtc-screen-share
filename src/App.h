#pragma once

#include <imgui.h>
#include <stdint.h>
#include <memory>
#include <string>
#include "capture/GraphicsCaptureFactory.h"
#include "platform/IWindow.h"
#include "platform/IRenderer.h"
#include "platform/ITexture.h"

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
	void RenderUI();
	void OnFrameArrived(const FrameData& frame);
	void CreateCaptureTexture(int width, int height);
	void OnWindowEvent(const WindowEvent& event);

private:
	static App *s_Instance;

	// Platform abstraction
	std::unique_ptr<IWindow> m_window;
	std::unique_ptr<IRenderer> m_renderer;

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
	std::unique_ptr<ITexture> m_captureTexture;
};