#include "ImGuiManager.h"

#include "IWindow.h"
#include "IRenderer.h"

#include <imgui.h>
#if PLATFORM_WINDOWS
#include <windows.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#endif

bool ImGuiManager::Initialize(IWindow *window, IRenderer *renderer)
{
	if (!window || !renderer)
		return false;

	// if (!m_device || !m_context)
	// 	return false;

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

	ImGuiIO &io = ImGui::GetIO();
#if PLATFORM_WINDOWS
	HWND hwnd = static_cast<HWND>(window->GetNativeHandle());

	auto dxDevice = static_cast<ID3D11Device *>(renderer->GetDevice());
	auto dxContext = static_cast<ID3D11DeviceContext *>(renderer->GetDeviceContext());

	// Check if backends are already initialized
	if (!io.BackendPlatformUserData)
	{
		ImGui_ImplWin32_Init(hwnd);
	}

	if (!io.BackendRendererUserData)
	{
		ImGui_ImplDX11_Init(dxDevice, dxContext);
	}
#endif
	return true;
}

void ImGuiManager::NewFrame()
{
#if PLATFORM_WINDOWS
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
#endif
	ImGui::NewFrame();

	// Enable docking over the main viewport
	if (m_dockingEnabled)
	{
		ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
	}
}

void ImGuiManager::Render()
{
	ImGuiIO &io = ImGui::GetIO();

	// Rendering
	ImGui::Render();

	// Render ImGui
#if PLATFORM_WINDOWS
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

	// Update and Render additional Platform Windows
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
#if PLATFORM_WINDOWS
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
#endif
	}
}
void ImGuiManager::Shutdown()
{
	if (ImGui::GetCurrentContext())
	{
		ImGuiIO &io = ImGui::GetIO();

		// Only shutdown backends if they were initialized
		if (io.BackendRendererUserData)
		{
#if PLATFORM_WINDOWS
			ImGui_ImplDX11_Shutdown();
#elif __APPLE__
			ImGui_ImplMetal_Shutdown();
#elif __linux__
			ImGui_ImplOpenGL3_Shutdown();
#endif
		}

		if (io.BackendPlatformUserData)
		{
#if PLATFORM_WINDOWS
			ImGui_ImplWin32_Shutdown();
#elif __APPLE__
			ImGui_ImplOSX_Shutdown();
#elif __linux__
			ImGui_ImplGlfw_Shutdown();
#endif
		}

		ImGui::DestroyContext();
	}
}
