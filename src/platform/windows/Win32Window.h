#pragma once

#include "../IWindow.h"
#include <windows.h>
#include <memory>

class IRenderer;

class Win32Window : public IWindow
{
public:
    Win32Window(const WindowConfig& config);
    ~Win32Window() override;

    void Shutdown() override;

    void PollEvents() override;
    bool ShouldClose() const override;

    void SetEventCallback(const WindowEventCallback& callback) override;
    
    void SetTitle(const std::string& title) override;
    void SetSize(int width, int height) override;
    void GetSize(int& width, int& height) const override;
    
    void* GetNativeHandle() const override { return m_hwnd; }

    std::string_view GetPlatformName() const noexcept override { return "Win32"; }

private:
    bool Initialize(const WindowConfig& config);
    
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void HandleWindowMessage(UINT msg, WPARAM wParam, LPARAM lParam);
    
private:
    HWND m_hwnd = nullptr;
    WNDCLASSEXW m_windowClass = {};
    bool m_shouldClose = false;
    int m_width = 0;
    int m_height = 0;
    
    WindowEventCallback m_eventCallback;
    
    static Win32Window* s_instance; // For window proc callback
};