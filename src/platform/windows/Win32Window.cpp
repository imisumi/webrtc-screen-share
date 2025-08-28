#include "Win32Window.h"
#include <iostream>
#include <imgui.h>
#include <imgui_impl_win32.h>

// For ImGui Win32 backend
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Win32Window* Win32Window::s_instance = nullptr;

Win32Window::Win32Window()
{
    s_instance = this;
}

Win32Window::~Win32Window()
{
    Shutdown();
    s_instance = nullptr;
}

bool Win32Window::Initialize(const WindowConfig& config)
{
    // Register window class
    m_windowClass = {};
    m_windowClass.cbSize = sizeof(WNDCLASSEXW);
    m_windowClass.style = CS_CLASSDC;
    m_windowClass.lpfnWndProc = WindowProc;
    m_windowClass.hInstance = GetModuleHandle(nullptr);
    m_windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    m_windowClass.lpszClassName = L"Win32WindowClass";

    if (!RegisterClassExW(&m_windowClass))
    {
        std::cout << "Failed to register Win32 window class" << std::endl;
        return false;
    }

    // Convert title to wide string
    std::wstring wideTitle(config.title.begin(), config.title.end());

    // Create window
    DWORD style = WS_OVERLAPPEDWINDOW;
    if (!config.resizable)
        style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);

    RECT rect = { 0, 0, config.width, config.height };
    AdjustWindowRect(&rect, style, FALSE);

    m_hwnd = CreateWindowW(
        m_windowClass.lpszClassName,
        wideTitle.c_str(),
        style,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left,
        rect.bottom - rect.top,
        nullptr, nullptr,
        m_windowClass.hInstance,
        nullptr
    );

    if (!m_hwnd)
    {
        std::cout << "Failed to create Win32 window" << std::endl;
        UnregisterClassW(m_windowClass.lpszClassName, m_windowClass.hInstance);
        return false;
    }

    m_width = config.width;
    m_height = config.height;


    // Show window
    ShowWindow(m_hwnd, SW_SHOWDEFAULT);
    UpdateWindow(m_hwnd);

    std::cout << "Win32Window initialized: " << config.width << "x" << config.height << std::endl;
    return true;
}

void Win32Window::Shutdown()
{
    if (m_hwnd)
    {
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }

    if (m_windowClass.lpszClassName)
    {
        UnregisterClassW(m_windowClass.lpszClassName, m_windowClass.hInstance);
        m_windowClass = {};
    }
}

void Win32Window::PollEvents()
{
    MSG msg;
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}


bool Win32Window::ShouldClose() const
{
    return m_shouldClose;
}

void Win32Window::SetEventCallback(const WindowEventCallback& callback)
{
    m_eventCallback = callback;
}

void Win32Window::SetTitle(const std::string& title)
{
    if (m_hwnd)
    {
        std::wstring wideTitle(title.begin(), title.end());
        SetWindowTextW(m_hwnd, wideTitle.c_str());
    }
}

void Win32Window::SetSize(int width, int height)
{
    if (m_hwnd)
    {
        RECT rect = { 0, 0, width, height };
        AdjustWindowRect(&rect, GetWindowLong(m_hwnd, GWL_STYLE), FALSE);
        SetWindowPos(m_hwnd, nullptr, 0, 0, 
                    rect.right - rect.left, 
                    rect.bottom - rect.top, 
                    SWP_NOMOVE | SWP_NOZORDER);
    }
}

void Win32Window::GetSize(int& width, int& height) const
{
    width = m_width;
    height = m_height;
}


LRESULT CALLBACK Win32Window::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // Let ImGui handle the message first
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
        return true;

    if (s_instance)
    {
        s_instance->HandleWindowMessage(msg, wParam, lParam);
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void Win32Window::HandleWindowMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
    WindowEvent event;
    bool hasEvent = false;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam != SIZE_MINIMIZED)
        {
            m_width = LOWORD(lParam);
            m_height = HIWORD(lParam);
            
            if (m_eventCallback)
            {
                event.type = WindowEvent::Resize;
                event.resize.width = m_width;
                event.resize.height = m_height;
                hasEvent = true;
            }
        }
        break;

    case WM_CLOSE:
        m_shouldClose = true;
        if (m_eventCallback)
        {
            event.type = WindowEvent::Close;
            hasEvent = true;
        }
        break;

    case WM_KEYDOWN:
    case WM_KEYUP:
        if (m_eventCallback)
        {
            event.type = (msg == WM_KEYDOWN) ? WindowEvent::KeyPress : WindowEvent::KeyRelease;
            event.keyboard.key = static_cast<KeyCode>(wParam); // Simple mapping for now
            hasEvent = true;
        }
        break;

    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
        if (m_eventCallback)
        {
            event.type = (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN) 
                        ? WindowEvent::MousePress : WindowEvent::MouseRelease;
            
            if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP)
                event.mouse.button = MouseButton::Left;
            else if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP)
                event.mouse.button = MouseButton::Right;
            else
                event.mouse.button = MouseButton::Middle;

            event.mouse.x = LOWORD(lParam);
            event.mouse.y = HIWORD(lParam);
            hasEvent = true;
        }
        break;

    case WM_MOUSEMOVE:
        if (m_eventCallback)
        {
            event.type = WindowEvent::MouseMove;
            event.mouseMove.x = LOWORD(lParam);
            event.mouseMove.y = HIWORD(lParam);
            hasEvent = true;
        }
        break;
    }

    if (hasEvent && m_eventCallback)
    {
        m_eventCallback(event);
    }
}