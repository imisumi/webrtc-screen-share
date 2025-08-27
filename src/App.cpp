#include "App.h"
#include <assert.h>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>
#include <stdio.h>
#include <stdexcept>

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
	if (ImGuiCol_TabActive != ImGuiCol_TabSelected)
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

static void glfw_error_callback(int error, const char *description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

App *App::s_Instance = nullptr;

App::App()
{
	assert(s_Instance == nullptr && "App already exists!");
	s_Instance = this;
	// Make process DPI aware and obtain main monitor scale
	ImGui_ImplWin32_EnableDpiAwareness();
	float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY));

	// Create application window
	WNDCLASSEXW wc = {sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr};
	::RegisterClassExW(&wc);
	m_hwnd = ::CreateWindowW(wc.lpszClassName, L"Dear ImGui DirectX11 Example", WS_OVERLAPPEDWINDOW, 100, 100, (int)(m_width * main_scale), (int)(m_height * main_scale), nullptr, nullptr, wc.hInstance, nullptr);

	// Initialize Direct3D
	if (!CreateDeviceD3D(m_hwnd))
	{
		CleanupDeviceD3D();
		::UnregisterClassW(wc.lpszClassName, wc.hInstance);
		throw std::runtime_error("Failed to create D3D device!");
	}

	// Show the window
	::ShowWindow(m_hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(m_hwnd);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;	  // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;	  // Enable Multi-Viewport / Platform Windows
	// io.ConfigViewportsNoAutoMerge = true;
	// io.ConfigViewportsNoTaskBarIcon = true;
	// io.ConfigViewportsNoDefaultParent = true;
	// io.ConfigDockingAlwaysTabBar = true;
	// io.ConfigDockingTransparentPayload = true;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// ImGui::StyleColorsLight();

	// Setup scaling
	ImGuiStyle &style = ImGui::GetStyle();
	style.ScaleAllSizes(main_scale);   // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
	style.FontScaleDpi = main_scale;   // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)
	io.ConfigDpiScaleFonts = true;	   // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. This will scale fonts but _NOT_ scale sizes/padding for now.
	io.ConfigDpiScaleViewports = true; // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(m_hwnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	// style.FontSizeBase = 20.0f;
	// io.Fonts->AddFontDefault();
	// io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf");
	// io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
	// io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
	// io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
	// ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf");
	// IM_ASSERT(font != nullptr);

	// Initialize Graphics Capture API
	m_graphicsCapture = GraphicsCaptureFactory::Create();
	if (m_graphicsCapture && m_graphicsCapture->Initialize())
	{
		printf("Graphics Capture API (%s) initialized successfully!\n", m_graphicsCapture->GetPlatformName());
		
		// Set shared D3D device
		if (m_graphicsCapture->SetD3DDevice(g_pd3dDevice))
		{
			printf("Shared D3D device set successfully!\n");
		}
		else
		{
			printf("Failed to set shared D3D device!\n");
		}
		
		// Set up frame callback
		m_graphicsCapture->SetFrameCallback([this](const FrameData& frame) {
			this->OnFrameArrived(frame);
		});
	}
	else
	{
		printf("Failed to initialize Graphics Capture API on %s\n", GraphicsCaptureFactory::GetCurrentPlatform());
	}
}

App::~App()
{
	// Cleanup capture resources
	if (m_captureSRV)
	{
		m_captureSRV->Release();
		m_captureSRV = nullptr;
	}
	if (m_captureTexture)
	{
		m_captureTexture->Release();
		m_captureTexture = nullptr;
	}

	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(m_hwnd);
	::UnregisterClassW(m_wc.lpszClassName, m_wc.hInstance);
}

void App::run()
{
	// Main loop
	bool done = false;
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	while (!done)
	{
		// Poll and handle messages (inputs, window resize, etc.)
		// See the WndProc() function below for our to dispatch events to the Win32 backend.
		MSG msg;
		while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				done = true;
		}
		if (done)
			break;

		// Handle window being minimized or screen locked
		if (g_SwapChainOccluded && g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
		{
			::Sleep(10);
			continue;
		}
		g_SwapChainOccluded = false;

		// Handle window resize (we don't resize directly in the WM_SIZE handler)
		if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
		{
			CleanupRenderTarget();
			g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
			g_ResizeWidth = g_ResizeHeight = 0;
			CreateRenderTarget();
		}

		// Start the Dear ImGui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (m_show_demo_window)
			ImGui::ShowDemoWindow(&m_show_demo_window);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");			 // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &m_show_demo_window); // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &m_show_another_window);

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
					for (const auto& monitor : monitors)
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
							printf("Selected monitor: %s\n", monitor.name.c_str());
							
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
					for (const auto& window : windows)
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
								printf("Selected window: %s (PID: %d)\n", window.title.c_str(), window.processId);
								
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
					
					// Quality setting
					const char* qualities[] = { "Low", "Medium", "High" };
					int currentQuality = static_cast<int>(config.quality);
					if (ImGui::Combo("Quality", &currentQuality, qualities, 3))
					{
						config.quality = static_cast<CaptureQuality>(currentQuality);
						m_graphicsCapture->SetCaptureConfig(config);
					}
					
					// FPS setting
					if (ImGui::SliderInt("Target FPS", &config.targetFps, 15, 120))
					{
						m_graphicsCapture->SetCaptureConfig(config);
					}
					
					// Cursor and borders
					if (ImGui::Checkbox("Include Cursor", &config.includeCursor))
					{
						m_graphicsCapture->SetCaptureConfig(config);
					}
					
					if (ImGui::Checkbox("Include Borders", &config.includeBorders))
					{
						m_graphicsCapture->SetCaptureConfig(config);
					}
					
					// Statistics
					ImGui::Spacing();
					auto stats = m_graphicsCapture->GetStatistics();
					ImGui::Text("Statistics:");
					ImGui::Text("  Frames Captured: %llu", stats.framesCapture);
					ImGui::Text("  Frames Dropped: %llu", stats.framesDropped);
					ImGui::Text("  Average FPS: %.1f", stats.averageFps);
					
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

			if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
			ImGui::End();
		}

		// 3. Show another simple window.
		if (m_show_another_window)
		{
			ImGui::Begin("Another Window", &m_show_another_window); // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				m_show_another_window = false;
			ImGui::End();
		}

		// 4. Always show live mirror window
		{
			ImGui::Begin("Live Mirror");
			
			if (m_graphicsCapture && m_graphicsCapture->IsCapturing() && m_captureSRV)
			{
				// Get available space
				ImVec2 available = ImGui::GetContentRegionAvail();
				available.y -= 30; // Leave space for status text
				
				// Calculate size maintaining aspect ratio
				float aspectRatio = (float)m_captureWidth / (float)m_captureHeight;
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
				
				ImGui::Image((void*)m_captureSRV, ImVec2(displayWidth, displayHeight));
				
				// Show info
				ImGui::Text("Resolution: %dx%d | Source: %s", 
					m_captureWidth, m_captureHeight, 
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

		// Rendering
		ImGui::Render();
		const float clear_color_with_alpha[4] = {m_clear_color.x * m_clear_color.w, m_clear_color.y * m_clear_color.w, m_clear_color.z * m_clear_color.w, m_clear_color.w};
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
		g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		// Present
		HRESULT hr = g_pSwapChain->Present(1, 0); // Present with vsync
		// HRESULT hr = g_pSwapChain->Present(0, 0); // Present without vsync
		g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
	}
}

// Helper functions
bool App::CreateDeviceD3D(HWND hWnd)
{
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	// createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_0,
	};
	HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
	if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
		res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
	if (res != S_OK)
		return false;

	CreateRenderTarget();
	return true;
}

void App::CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain)
	{
		g_pSwapChain->Release();
		g_pSwapChain = nullptr;
	}
	if (g_pd3dDeviceContext)
	{
		g_pd3dDeviceContext->Release();
		g_pd3dDeviceContext = nullptr;
	}
	if (g_pd3dDevice)
	{
		g_pd3dDevice->Release();
		g_pd3dDevice = nullptr;
	}
}

void App::CreateRenderTarget()
{
	ID3D11Texture2D *pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
	pBackBuffer->Release();
}

void App::CleanupRenderTarget()
{
	if (g_mainRenderTargetView)
	{
		g_mainRenderTargetView->Release();
		g_mainRenderTargetView = nullptr;
	}
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI App::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
			return 0;
		s_Instance->g_ResizeWidth = (UINT)LOWORD(lParam);   // Width is LOWORD
		s_Instance->g_ResizeHeight = (UINT)HIWORD(lParam);  // Height is HIWORD
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

void App::OnFrameArrived(const FrameData& frame)
{
	if (!frame.data || frame.size == 0)
		return;

	// Create texture if needed or if size changed
	if (!m_captureTexture || m_captureWidth != frame.width || m_captureHeight != frame.height)
	{
		CreateCaptureTexture(frame.width, frame.height);
	}

	if (m_captureTexture && g_pd3dDeviceContext)
	{
		// Update texture with new frame data
		D3D11_MAPPED_SUBRESOURCE mapped;
		HRESULT hr = g_pd3dDeviceContext->Map(m_captureTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
		if (SUCCEEDED(hr))
		{
			// Copy frame data to texture, handling stride properly
			uint8_t* src = static_cast<uint8_t*>(frame.data);
			uint8_t* dst = static_cast<uint8_t*>(mapped.pData);
			
			// Use the actual bytes per pixel based on captured data
			int bytesPerPixel = 4; // BGRA format
			int srcRowBytes = frame.width * bytesPerPixel;
			
			// Debug info for first frame
			static bool firstFrame = true;
			if (firstFrame)
			{
				printf("Frame: %dx%d, stride: %d, expected: %d, mapped pitch: %d\n", 
				       frame.width, frame.height, frame.stride, srcRowBytes, mapped.RowPitch);
				firstFrame = false;
			}
			
			for (int y = 0; y < frame.height; ++y)
			{
				// Copy only the actual pixel data, not any padding
				memcpy(dst + y * mapped.RowPitch, 
				       src + y * frame.stride, 
				       srcRowBytes);
			}
			
			g_pd3dDeviceContext->Unmap(m_captureTexture, 0);
		}
	}
}

void App::CreateCaptureTexture(int width, int height)
{
	// Clean up old resources
	if (m_captureSRV)
	{
		m_captureSRV->Release();
		m_captureSRV = nullptr;
	}
	if (m_captureTexture)
	{
		m_captureTexture->Release();
		m_captureTexture = nullptr;
	}

	if (!g_pd3dDevice)
		return;

	// Create new texture
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // BGRA format
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DYNAMIC;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = g_pd3dDevice->CreateTexture2D(&textureDesc, nullptr, &m_captureTexture);
	if (SUCCEEDED(hr))
	{
		// Create shader resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		hr = g_pd3dDevice->CreateShaderResourceView(m_captureTexture, &srvDesc, &m_captureSRV);
		if (SUCCEEDED(hr))
		{
			m_captureWidth = width;
			m_captureHeight = height;
			printf("Created capture texture: %dx%d\n", width, height);
		}
	}

	if (FAILED(hr))
	{
		printf("Failed to create capture texture\n");
	}
}
