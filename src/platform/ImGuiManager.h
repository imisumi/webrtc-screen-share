#pragma once

class IWindow;
class IRenderer;

class ImGuiManager
{
public:
	ImGuiManager() = default;
	~ImGuiManager() = default;
	bool Initialize(IWindow *window, IRenderer *renderer);
	void NewFrame();
	void Render();
	void Shutdown();

private:
	bool m_initialized = false;
	IWindow *m_window;
	IRenderer *m_renderer;
	bool m_dockingEnabled = true;
};

// Application just does:
// m_imguiManager.Initialize(m_window.get(), m_renderer.get());