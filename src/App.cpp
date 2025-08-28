#include "App.h"
#include <assert.h>
#include "imgui.h"
#include <iostream>
#include <format>
#include <stdexcept>
#include "platform/WindowFactory.h"
#include "platform/TextureFactory.h"

#ifdef PLATFORM_WINDOWS
#include <d3d11.h>
#include <imgui_impl_win32.h>
#include "platform/windows/D3D11Texture.h"
#endif

static void SetDarkThemeColors()
{
	ImGuiStyle &style = ImGui::GetStyle();
	ImVec4 *colors = style.Colors;

	// Keep the same spacing and rounding
	style.WindowRounding = 6.0f;
	style.WindowBorderSize = 1.0f;
	style.WindowPadding = ImVec2(12, 12);
	style.FramePadding = ImVec2(6, 4);
	style.FrameRounding = 4.0f;
	style.ItemSpacing = ImVec2(8, 6);
	style.ItemInnerSpacing = ImVec2(6, 4);
	style.IndentSpacing = 22.0f;
	style.ScrollbarSize = 14.0f;
	style.ScrollbarRounding = 8.0f;
	style.GrabMinSize = 12.0f;
	style.GrabRounding = 3.0f;
	style.PopupRounding = 4.0f;

	// Base colors
	colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);			// Light grey text (not pure white)
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f); // Medium grey for disabled
	colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.12f, 0.12f, 0.95f);		// Dark m_window background
	colors[ImGuiCol_ChildBg] = ImVec4(0.12f, 0.12f, 0.12f, 0.95f);		// Match m_window background
	colors[ImGuiCol_PopupBg] = ImVec4(0.14f, 0.14f, 0.14f, 0.95f);		// Slightly darker than m_window
	colors[ImGuiCol_Border] = ImVec4(0.25f, 0.25f, 0.25f, 0.50f);		// Dark grey border
	colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);		// No shadow

	// Frame colors
	colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.16f, 0.16f, 0.95f);		  // Dark element backgrounds
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.20f, 0.20f, 0.95f); // Slightly lighter on hover
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);  // Even lighter when active

	// Title bar colors
	colors[ImGuiCol_TitleBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);			// Dark grey inactive title
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);	// Slightly lighter active title
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.15f, 0.15f, 0.15f, 0.75f); // Transparent when collapsed
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);		// Slightly darker than title

	// Scrollbar colors
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.14f, 0.14f, 0.14f, 0.95f);			// Scrollbar background
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);		// Scrollbar grab
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f); // Scrollbar grab when hovered
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);	// Scrollbar grab when active

	// Widget colors
	colors[ImGuiCol_CheckMark] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);		// Light grey checkmark
	colors[ImGuiCol_SliderGrab] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);		// Slider grab
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f); // Slider grab when active
	colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.20f, 0.80f);			// Dark grey buttons
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);	// Slightly lighter on hover
	colors[ImGuiCol_ButtonActive] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);		// Even lighter when active

	// Header colors (TreeNode, Selectable, etc)
	colors[ImGuiCol_Header] = ImVec4(0.20f, 0.20f, 0.20f, 0.76f);		 // Pure dark grey
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.25f, 0.80f); // Slightly lighter on hover
	colors[ImGuiCol_HeaderActive] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);	 // Even lighter when active

	// Separator
	colors[ImGuiCol_Separator] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);		// Separator color
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f); // Separator when hovered
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);	// Separator when active

	// Resize grip
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.25f, 0.25f, 0.25f, 0.50f);		 // Resize grip
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.30f, 0.30f, 0.30f, 0.75f); // Resize grip when hovered
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);	 // Resize grip when active

	// Text input cursor
	colors[ImGuiCol_InputTextCursor] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f); // Text input cursor

	// ALL TAB COLORS (both old and new names)
	// Using the newer tab color naming from your enum
	colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.15f, 0.15f, 0.86f);						 // Unselected tab
	colors[ImGuiCol_TabHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.80f);				 // Tab when hovered
	colors[ImGuiCol_TabSelected] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);				 // Selected tab
	colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);		 // Selected tab overline
	colors[ImGuiCol_TabDimmed] = ImVec4(0.13f, 0.13f, 0.13f, 0.86f);				 // Dimmed/unfocused tab
	colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);		 // Selected but unfocused tab
	colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f); // Overline of unfocused selected tab

	// For backward compatibility with older ImGui versions
	// These might be what your version is using
	if constexpr (ImGuiCol_TabActive != ImGuiCol_TabSelected)
	{																			  // Only set if they're different (to avoid warnings)
		colors[ImGuiCol_TabActive] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);		  // Active tab (old name)
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.13f, 0.13f, 0.13f, 0.86f);		  // Unfocused tab (old name)
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f); // Unfocused active tab (old name)
	}

	// Docking colors
	colors[ImGuiCol_DockingPreview] = ImVec4(0.30f, 0.30f, 0.30f, 0.40f); // Preview when docking
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f); // Empty docking space

	// Plot colors
	colors[ImGuiCol_PlotLines] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);			// Plot lines
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);		// Plot lines when hovered
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);		// Plot histogram
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f); // Plot histogram when hovered

	// Table colors
	colors[ImGuiCol_TableHeaderBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);	 // Table header background
	colors[ImGuiCol_TableBorderStrong] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f); // Table outer borders
	colors[ImGuiCol_TableBorderLight] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);	 // Table inner borders
	colors[ImGuiCol_TableRowBg] = ImVec4(0.14f, 0.14f, 0.14f, 0.90f);		 // Table row background (even)
	colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.16f, 0.16f, 0.16f, 0.90f);	 // Table row background (odd)

	// Miscellaneous
	colors[ImGuiCol_TextLink] = ImVec4(0.55f, 0.55f, 0.55f, 1.00f);				 // Light grey for links (not blue)
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.30f, 0.30f, 0.30f, 0.35f);		 // Light grey selection background
	colors[ImGuiCol_TreeLines] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);			 // Tree node lines
	colors[ImGuiCol_DragDropTarget] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);		 // Drag and drop target
	colors[ImGuiCol_NavCursor] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);			 // Navigation cursor
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.40f, 0.40f, 0.40f, 0.70f); // Nav windowing highlight
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.20f);	 // Nav windowing dim
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.15f, 0.15f, 0.15f, 0.75f);		 // Modal m_window dim
}

App *App::s_Instance = nullptr;

App::App()
{
	assert(s_Instance == nullptr && "App already exists!");
	s_Instance = this;
}

App::~App()
{
	// Cleanup capture resources - handled by smart pointer destructor
	m_captureTexture.reset();

	// Shutdown platform abstractions (window first to stop messages, then renderer)
	if (m_window)
		m_window->Shutdown();
	if (m_renderer)
		m_renderer->Shutdown();

	s_Instance = nullptr;
}

void App::run()
{
	// Create platform-specific window and renderer
	m_window = WindowFactory::Create();
	if (!m_window)
	{
		throw std::runtime_error("Failed to create window!");
	}

	WindowConfig config;
	config.title = "WebRTC Screen Share";
	config.width = m_width;
	config.height = m_height;
	config.resizable = true;
	config.vsync = true;

	if (!m_window->Initialize(config))
	{
		throw std::runtime_error("Failed to initialize window!");
	}

	// Set up window event callback
	m_window->SetEventCallback([this](const WindowEvent &event)
							   { this->OnWindowEvent(event); });

	m_renderer = WindowFactory::CreateRenderer();
	if (!m_renderer || !m_renderer->Initialize(m_window.get()))
	{
		throw std::runtime_error("Failed to create or initialize renderer!");
	}

	// Initialize ImGui (this creates the context)
	if (!m_renderer->InitializeImGui())
	{
		throw std::runtime_error("Failed to initialize ImGui!");
	}

	// Enable DPI awareness (after ImGui context exists)
#ifdef PLATFORM_WINDOWS
	ImGui_ImplWin32_EnableDpiAwareness();
#endif

	// Apply dark theme
	SetDarkThemeColors();

	std::cout << std::format("Platform: {}, Renderer: {}\n", m_window->GetPlatformName(), m_renderer->GetRendererName());

	// Initialize Graphics Capture API
	m_graphicsCapture = GraphicsCaptureFactory::Create();
	if (m_graphicsCapture && m_graphicsCapture->Initialize())
	{
		std::cout << std::format("Graphics Capture API ({}) initialized successfully!\n", m_graphicsCapture->GetPlatformName());

		// Set shared device
		void *device = m_renderer->GetDevice();
		if (m_graphicsCapture->SetD3DDevice(device))
		{
			std::cout << "Shared device set successfully!\n";
		}
		else
		{
			std::cout << "Failed to set shared device!\n";
		}

		// Set up frame callback
		m_graphicsCapture->SetFrameCallback([this](const FrameData &frame)
											{ this->OnFrameArrived(frame); });
	}
	else
	{
		std::cout << std::format("Failed to initialize Graphics Capture API on {}\n", GraphicsCaptureFactory::GetCurrentPlatform());
	}

	// Main loop
	while (!m_window->ShouldClose())
	{
		// Poll events
		m_window->PollEvents();

		// Begin frame
		m_renderer->BeginFrame();
		m_renderer->Clear(m_clear_color.x * m_clear_color.w,
						  m_clear_color.y * m_clear_color.w,
						  m_clear_color.z * m_clear_color.w,
						  m_clear_color.w);

		// Render ImGui UI
		RenderUI();

		// End frame
		m_renderer->EndFrame();
		m_renderer->Present();
	}
}

void App::RenderUI()
{
	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
	{
		static float f = 0.0f;

		ImGui::Begin("Stats");

		// Statistics
		ImGui::Spacing();
		ImGuiIO &io = ImGui::GetIO();
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		if (m_graphicsCapture && m_graphicsCapture->IsInitialized())
		{
			auto stats = m_graphicsCapture->GetStatistics();
			ImGui::Text("Statistics:");
			ImGui::Text("  Frames Captured: %llu", stats.framesCapture);
			ImGui::Text("  Frames Dropped: %llu", stats.framesDropped);
			ImGui::Text("  Average FPS: %.1f", stats.averageFps);
		}
		else
		{
			ImGui::Text("Statistics:");
			ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "  Graphics capture not initialized");
		}

		ImGui::End();

		ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text."); // Display some text (you can use a format strings too)

		// Graphics Capture section
		ImGui::Separator();
		ImGui::Text("Graphics Capture");

		if (m_graphicsCapture && m_graphicsCapture->IsInitialized())
		{
			ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "✓ %s", m_graphicsCapture->GetPlatformName());

			// Capture Source Selection
			ImGui::Spacing();
			ImGui::Text("Select Capture Source:");

			// Selected source display
			if (!m_selectedSourceId.empty())
			{
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f),
								   "Selected: %s %s",
								   m_selectedIsMonitor ? "📺" : "🪟",
								   m_selectedSourceName.c_str());
			}
			else
			{
				ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No source selected");
			}

			ImGui::Spacing();

			// Monitors section
			if (ImGui::CollapsingHeader("📺 Monitors", ImGuiTreeNodeFlags_DefaultOpen))
			{
				auto monitors = m_graphicsCapture->GetMonitors();
				for (const auto &monitor : monitors)
				{
					std::string label = monitor.name +
										(monitor.isPrimary ? " (Primary)" : "") +
										" - " + std::to_string(monitor.width) + "x" + std::to_string(monitor.height);

					bool isSelected = (m_selectedSourceId == monitor.id);

					if (ImGui::RadioButton(("##monitor_" + monitor.id).c_str(), isSelected))
					{
						// Stop current capture if any
						if (m_graphicsCapture->IsCapturing())
						{
							m_graphicsCapture->StopCapture();
						}

						m_selectedSourceId = monitor.id;
						m_selectedSourceName = label;
						m_selectedIsMonitor = true;
						std::cout << std::format("Selected monitor: {}\n", monitor.name);

						// Auto-start capture
						m_graphicsCapture->StartCapture(m_selectedSourceId);
					}

					ImGui::SameLine();
					ImGui::Text("%s", label.c_str());

					if (ImGui::IsItemHovered())
					{
						ImGui::SetTooltip("Position: %d,%d\nDPI Scale: %.1fx",
										  monitor.x, monitor.y, monitor.dpiScale);
					}
				}
			}

			// Windows section
			if (ImGui::CollapsingHeader("🪟 Applications", ImGuiTreeNodeFlags_DefaultOpen))
			{
				auto windows = m_graphicsCapture->GetWindows();

				ImGui::BeginChild("WindowSelection", ImVec2(0, 150), true);
				for (const auto &window : windows)
				{
					// Only filter out windows with no title
					if (!window.title.empty())
					{
						std::string label = window.title + " - " + window.processName;

						// Add status indicators
						std::string status = "";
						if (window.isMinimized)
							status += " [Minimized]";
						if (window.width < 100 || window.height < 100)
							status += " [Small]";

						std::string size_info = " (" + std::to_string(window.width) + "x" + std::to_string(window.height) + ")";
						std::string full_label = label + size_info + status;

						bool isSelected = (m_selectedSourceId == window.id);

						// Dim minimized windows slightly
						if (window.isMinimized)
						{
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
						}

						if (ImGui::RadioButton(("##window_" + window.id).c_str(), isSelected))
						{
							// Stop current capture if any
							if (m_graphicsCapture->IsCapturing())
							{
								m_graphicsCapture->StopCapture();
							}

							m_selectedSourceId = window.id;
							m_selectedSourceName = full_label;
							m_selectedIsMonitor = false;
							std::cout << std::format("Selected window: {} (PID: {})\n", window.title, window.processId);

							// Auto-start capture
							m_graphicsCapture->StartCapture(m_selectedSourceId);
						}

						ImGui::SameLine();
						ImGui::Text("%s", full_label.c_str());

						if (window.isMinimized)
						{
							ImGui::PopStyleColor();
						}

						if (ImGui::IsItemHovered())
						{
							ImGui::SetTooltip("Process: %s\nPID: %d\nPosition: %d,%d\nSize: %dx%d\nVisible: %s\nMinimized: %s",
											  window.processName.c_str(), window.processId,
											  window.x, window.y, window.width, window.height,
											  window.isVisible ? "Yes" : "No",
											  window.isMinimized ? "Yes" : "No");
						}
					}
				}
				ImGui::EndChild();
			}

			// Mirror status
			ImGui::Spacing();
			if (m_graphicsCapture->IsCapturing())
			{
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "● Live Mirror Active");
			}
			else
			{
				ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Select a source to start mirroring");
			}

			// Configuration
			ImGui::Spacing();
			if (ImGui::CollapsingHeader("Capture Settings"))
			{
				auto config = m_graphicsCapture->GetCaptureConfig();

				// Cursor and borders
				if (ImGui::Checkbox("Include Cursor", &config.includeCursor))
				{
					m_graphicsCapture->SetCaptureConfig(config);
				}

				if (ImGui::Checkbox("Include Borders", &config.includeBorders))
				{
					m_graphicsCapture->SetCaptureConfig(config);
				}

				// Cursor info
				if (m_graphicsCapture->IsCursorVisible())
				{
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "  Cursor: Visible");
				}
				else
				{
					ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "  Cursor: Hidden");
				}
			}
		}
		else
		{
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "✗ Graphics Capture Not Available (%s)", GraphicsCaptureFactory::GetCurrentPlatform());
		}

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);			   // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float *)&m_clear_color); // Edit 3 floats representing a color

		ImGui::End();
	}

	// 4. Always show live mirror window
	{
		ImGui::Begin("Live Mirror");

		if (m_graphicsCapture && m_graphicsCapture->IsCapturing() && m_captureTexture && m_captureTexture->IsValid())
		{
			// Get available space
			ImVec2 available = ImGui::GetContentRegionAvail();
			available.y -= 30; // Leave space for status text

			// Calculate size maintaining aspect ratio
			float aspectRatio = (float)m_captureTexture->GetWidth() / (float)m_captureTexture->GetHeight();
			float displayWidth = available.x;
			float displayHeight = available.x / aspectRatio;

			if (displayHeight > available.y)
			{
				displayHeight = available.y;
				displayWidth = available.y * aspectRatio;
			}

			// Center the image
			float offsetX = (available.x - displayWidth) * 0.5f;
			if (offsetX > 0)
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);

			ImGui::Image(m_captureTexture->GetShaderResourceView(), ImVec2(displayWidth, displayHeight));

			// Show info
			ImGui::Text("Resolution: %dx%d | Source: %s",
						m_captureTexture->GetWidth(), m_captureTexture->GetHeight(),
						m_selectedSourceName.c_str());
		}
		else
		{
			ImVec2 available = ImGui::GetContentRegionAvail();
			float centerY = available.y * 0.5f - 20;

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + centerY);

			float textWidth = ImGui::CalcTextSize("No active mirror").x;
			float centerX = (available.x - textWidth) * 0.5f;
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + centerX);

			ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No active mirror");
		}

		ImGui::End();
	}
}

void App::OnWindowEvent(const WindowEvent &event)
{
	switch (event.type)
	{
	case WindowEvent::Resize:
		if (m_renderer)
		{
			m_renderer->OnWindowResize(event.resize.width, event.resize.height);
		}
		m_width = event.resize.width;
		m_height = event.resize.height;
		break;
	case WindowEvent::Close:
		// Window will close automatically
		break;
	default:
		break;
	}
}

void App::OnFrameArrived(const FrameData &frame)
{
	if (!frame.data || frame.size == 0)
		return;

	// Create texture if needed or if size changed
	if (!m_captureTexture ||
		m_captureTexture->GetWidth() != frame.width ||
		m_captureTexture->GetHeight() != frame.height)
	{
		CreateCaptureTexture(frame.width, frame.height);
	}

	// Update texture with new frame data
	if (m_captureTexture && m_captureTexture->IsValid())
	{
		// Debug info for first frame
		static bool firstFrame = true;
		if (firstFrame)
		{
			std::cout << std::format("Frame: {}x{}, stride: {}\n",
									 frame.width, frame.height, frame.stride);
			firstFrame = false;
		}

		// Update the texture data
		bool success = m_captureTexture->Update(frame.data, frame.size, frame.stride);
		if (!success)
		{
			std::cout << "Failed to update capture texture\n";
		}
	}
}

void App::CreateCaptureTexture(int width, int height)
{
	// Clean up old texture
	m_captureTexture.reset();

	// Create new texture using factory
	m_captureTexture = TextureFactory::Create();
	if (!m_captureTexture)
	{
		std::cout << "Failed to create texture from factory\n";
		return;
	}

#ifdef PLATFORM_WINDOWS
	// Set D3D11 device for the texture
	auto *d3dTexture = static_cast<D3D11Texture *>(m_captureTexture.get());
	auto *d3dDevice = static_cast<ID3D11Device *>(m_renderer->GetDevice());
	if (!d3dDevice)
	{
		std::cout << "No D3D11 device available\n";
		m_captureTexture.reset();
		return;
	}

	d3dTexture->SetDevice(d3dDevice);
#endif

	// Create texture with proper descriptor
	TextureDesc desc;
	desc.width = width;
	desc.height = height;
	desc.format = TextureFormat::BGRA8;
	desc.usage = TextureUsage::Dynamic;

	if (!m_captureTexture->Create(desc))
	{
		std::cout << "Failed to create capture texture\n";
		m_captureTexture.reset();
	}
}