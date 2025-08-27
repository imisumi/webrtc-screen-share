#include "WindowsGraphicsCapture.h"

#ifdef PLATFORM_WINDOWS

#include <iostream>
#include <vector>
#include <shellscalingapi.h>
#include <psapi.h>
#include <winrt/base.h>
#include <winrt/Windows.Graphics.Capture.h>
#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>
#include <windows.graphics.capture.interop.h>
#include <windows.graphics.directx.direct3d11.interop.h>
#include <Windows.Graphics.DirectX.Direct3D11.interop.h>
#include <d3d11.h>
#include <dxgi.h>
#include <dxgi1_2.h>

// Include the IDirect3DDxgiInterfaceAccess interface
#include <Windows.Graphics.DirectX.Direct3D11.interop.h>

// Using aliases for long nested namespaces
using GraphicsCaptureItem = winrt::Windows::Graphics::Capture::GraphicsCaptureItem;
using GraphicsCaptureSession = winrt::Windows::Graphics::Capture::GraphicsCaptureSession;
using Direct3D11CaptureFramePool = winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool;
using IDirect3DDevice = winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice;

WindowsGraphicsCapture::WindowsGraphicsCapture()
{
}

WindowsGraphicsCapture::~WindowsGraphicsCapture()
{
    Shutdown();
}

bool WindowsGraphicsCapture::Initialize()
{
    try
    {
        // Initialize WinRT
        winrt::init_apartment();
        
        // Check if Graphics Capture is supported
        if (!IsSupported())
        {
            std::cout << "Windows Graphics Capture API is not supported on this system" << std::endl;
            return false;
        }

        std::cout << "Windows Graphics Capture API initialized successfully!" << std::endl;
        
        m_initialized = true;
        return true;
    }
    catch (...)
    {
        std::cout << "Failed to initialize Windows Graphics Capture API" << std::endl;
        return false;
    }
}

bool WindowsGraphicsCapture::SetD3DDevice(void* d3dDevice)
{
    try
    {
        if (!d3dDevice)
        {
            std::cout << "Invalid D3D device provided" << std::endl;
            return false;
        }

        // Convert ID3D11Device to WinRT IDirect3DDevice
        ID3D11Device* d3d11Device = static_cast<ID3D11Device*>(d3dDevice);
        
        // Get DXGI device from D3D11 device
        winrt::com_ptr<IDXGIDevice> dxgiDevice;
        HRESULT hr = d3d11Device->QueryInterface(winrt::guid_of<IDXGIDevice>(), dxgiDevice.put_void());
        if (FAILED(hr))
        {
            std::cout << "Failed to get DXGI device" << std::endl;
            return false;
        }

        // Create WinRT Direct3D device
        hr = CreateDirect3D11DeviceFromDXGIDevice(dxgiDevice.get(), reinterpret_cast<IInspectable**>(winrt::put_abi(m_device)));
        if (SUCCEEDED(hr))
        {
            std::cout << "Successfully created WinRT D3D device from shared device" << std::endl;
            return true;
        }
        else
        {
            std::cout << "Failed to create WinRT D3D device" << std::endl;
            return false;
        }
    }
    catch (...)
    {
        std::cout << "Exception in SetD3DDevice" << std::endl;
        return false;
    }
}

void WindowsGraphicsCapture::Shutdown()
{
    StopCapture();
    m_initialized = false;
}

bool WindowsGraphicsCapture::IsSupported() const
{
    try
    {
        return GraphicsCaptureSession::IsSupported();
    }
    catch (...)
    {
        return false;
    }
}

std::vector<Monitor> WindowsGraphicsCapture::GetMonitors() const
{
    std::vector<Monitor> monitors;
    
    if (!m_initialized)
        return monitors;

    EnumDisplayMonitors(nullptr, nullptr, [](HMONITOR hMonitor, HDC, LPRECT, LPARAM lParam) -> BOOL
    {
        auto* monitors = reinterpret_cast<std::vector<Monitor>*>(lParam);
        
        MONITORINFOEX info = {};
        info.cbSize = sizeof(MONITORINFOEX);
        
        if (GetMonitorInfo(hMonitor, &info))
        {
            Monitor monitor;
            monitor.id = std::to_string(reinterpret_cast<uintptr_t>(hMonitor));
            monitor.name = std::string(info.szDevice);
            monitor.x = info.rcMonitor.left;
            monitor.y = info.rcMonitor.top;
            monitor.width = info.rcMonitor.right - info.rcMonitor.left;
            monitor.height = info.rcMonitor.bottom - info.rcMonitor.top;
            monitor.isPrimary = (info.dwFlags & MONITORINFOF_PRIMARY) != 0;
            
            // Get DPI scaling
            UINT dpiX, dpiY;
            if (SUCCEEDED(GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY)))
            {
                monitor.dpiScale = dpiX / 96.0f;
            }
            else
            {
                monitor.dpiScale = 1.0f;
            }
            
            monitors->push_back(monitor);
        }
        
        return TRUE;
    }, reinterpret_cast<LPARAM>(&monitors));
    
    return monitors;
}

std::vector<Window> WindowsGraphicsCapture::GetWindows() const
{
    std::vector<Window> windows;
    
    if (!m_initialized)
        return windows;

    EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL
    {
        auto* windows = reinterpret_cast<std::vector<Window>*>(lParam);
        
        if (IsWindowVisible(hwnd) && GetWindowTextLength(hwnd) > 0)
        {
            Window window;
            window.id = std::to_string(reinterpret_cast<uintptr_t>(hwnd));
            
            // Get window title
            char title[256];
            GetWindowTextA(hwnd, title, sizeof(title));
            window.title = title;
            
            // Get process info
            DWORD processId;
            GetWindowThreadProcessId(hwnd, &processId);
            window.processId = processId;
            
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processId);
            if (hProcess)
            {
                char processName[MAX_PATH];
                DWORD size = sizeof(processName);
                if (QueryFullProcessImageNameA(hProcess, 0, processName, &size))
                {
                    // Extract just the filename
                    std::string fullPath(processName);
                    size_t lastSlash = fullPath.find_last_of("\\/");
                    window.processName = (lastSlash != std::string::npos) ? 
                        fullPath.substr(lastSlash + 1) : fullPath;
                }
                CloseHandle(hProcess);
            }
            
            // Get window position and size
            RECT rect;
            if (GetWindowRect(hwnd, &rect))
            {
                window.x = rect.left;
                window.y = rect.top;
                window.width = rect.right - rect.left;
                window.height = rect.bottom - rect.top;
            }
            
            window.isVisible = IsWindowVisible(hwnd);
            window.isMinimized = IsIconic(hwnd);
            
            windows->push_back(window);
        }
        
        return TRUE;
    }, reinterpret_cast<LPARAM>(&windows));
    
    return windows;
}

std::vector<CaptureSource> WindowsGraphicsCapture::GetAvailableSources() const
{
    std::vector<CaptureSource> sources;
    
    // Add monitors as sources
    auto monitors = GetMonitors();
    for (const auto& monitor : monitors)
    {
        CaptureSource source;
        source.id = monitor.id;
        source.name = monitor.name + (monitor.isPrimary ? " (Primary)" : "");
        source.isMonitor = true;
        source.width = monitor.width;
        source.height = monitor.height;
        sources.push_back(source);
    }
    
    // Add windows as sources
    auto windows = GetWindows();
    for (const auto& window : windows)
    {
        if (!window.isMinimized && window.width > 100 && window.height > 100)
        {
            CaptureSource source;
            source.id = window.id;
            source.name = window.title + " - " + window.processName;
            source.isMonitor = false;
            source.width = window.width;
            source.height = window.height;
            sources.push_back(source);
        }
    }
    
    return sources;
}

bool WindowsGraphicsCapture::StartCapture(const std::string& sourceId)
{
    if (!m_initialized || m_isCapturing)
        return false;

    try
    {
        std::cout << "Starting capture for source: " << sourceId << std::endl;
        
        // Convert sourceId back to window handle or monitor
        uintptr_t handleValue = std::stoull(sourceId);
        
        // Get dimensions based on source type
        int width = 1920;  // Default
        int height = 1080;
        
        // Try as window handle first
        HWND hwnd = reinterpret_cast<HWND>(handleValue);
        if (IsWindow(hwnd))
        {
            // Create capture item for window
            auto interop = winrt::get_activation_factory<GraphicsCaptureItem, IGraphicsCaptureItemInterop>();
            HRESULT hr = interop->CreateForWindow(hwnd, winrt::guid_of<GraphicsCaptureItem>(), winrt::put_abi(m_captureItem));
            
            if (SUCCEEDED(hr))
            {
                auto size = m_captureItem.Size();
                width = size.Width;
                height = size.Height;
                std::cout << "Window capture created: " << width << "x" << height << std::endl;
            }
            else
            {
                std::cout << "Failed to create window capture item" << std::endl;
                return false;
            }
        }
        else
        {
            // Try as monitor handle
            HMONITOR hMonitor = reinterpret_cast<HMONITOR>(handleValue);
            auto interop = winrt::get_activation_factory<GraphicsCaptureItem, IGraphicsCaptureItemInterop>();
            HRESULT hr = interop->CreateForMonitor(hMonitor, winrt::guid_of<GraphicsCaptureItem>(), winrt::put_abi(m_captureItem));
            
            if (SUCCEEDED(hr))
            {
                auto size = m_captureItem.Size();
                width = size.Width;
                height = size.Height;
                std::cout << "Monitor capture created: " << width << "x" << height << std::endl;
            }
            else
            {
                std::cout << "Failed to create monitor capture item" << std::endl;
                return false;
            }
        }
        
        // Create real capture session
        if (!m_device)
        {
            std::cout << "No D3D device available for capture" << std::endl;
            return false;
        }

        // Create frame pool for capture frames
        m_framePool = Direct3D11CaptureFramePool::Create(
            m_device,
            winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
            2, // Number of buffers
            {width, height}
        );
        
        // Set up frame arrived callback
        m_framePool.FrameArrived([this](auto&& sender, auto&& args) {
            this->OnFrameArrived();
        });
        
        // Create capture session
        m_session = m_framePool.CreateCaptureSession(m_captureItem);
        
        // Start capturing!
        m_session.StartCapture();
        
        std::cout << "Real capture session started: " << width << "x" << height << std::endl;
        
        m_isCapturing = true;
        return true;
    }
    catch (...)
    {
        std::cout << "Exception in StartCapture" << std::endl;
        return false;
    }
}

void WindowsGraphicsCapture::OnFrameArrived()
{
    if (!m_framePool)
        return;

    try
    {
        // Get the captured frame
        auto frame = m_framePool.TryGetNextFrame();
        if (!frame)
            return;

        // Extract pixel data from the captured frame
        auto size = frame.ContentSize();
        
        if (m_frameCallback)
        {
            try
            {
                // Get the native D3D11 texture from the WinRT surface using IDirect3DDxgiInterfaceAccess
                auto surfaceAccess = frame.Surface().as<Windows::Graphics::DirectX::Direct3D11::IDirect3DDxgiInterfaceAccess>();
                winrt::com_ptr<ID3D11Texture2D> nativeSurface;
                HRESULT hr = surfaceAccess->GetInterface(IID_PPV_ARGS(nativeSurface.put()));
                
                if (SUCCEEDED(hr) && nativeSurface)
                {
                    // Get the D3D11 device from our WinRT device
                    auto deviceAccess = m_device.as<Windows::Graphics::DirectX::Direct3D11::IDirect3DDxgiInterfaceAccess>();
                    winrt::com_ptr<ID3D11Device> nativeDevice;
                    hr = deviceAccess->GetInterface(IID_PPV_ARGS(nativeDevice.put()));
                    
                    if (SUCCEEDED(hr) && nativeDevice)
                    {
                        // Create a staging texture to copy and read the pixels
                        D3D11_TEXTURE2D_DESC desc;
                        nativeSurface->GetDesc(&desc);
                        
                        desc.Usage = D3D11_USAGE_STAGING;
                        desc.BindFlags = 0;
                        desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
                        desc.MiscFlags = 0;
                        
                        winrt::com_ptr<ID3D11DeviceContext> context;
                        nativeDevice->GetImmediateContext(context.put());
                        
                        winrt::com_ptr<ID3D11Texture2D> stagingTexture;
                        hr = nativeDevice->CreateTexture2D(&desc, nullptr, stagingTexture.put());
                        
                        if (SUCCEEDED(hr))
                        {
                            // Copy the captured frame to staging texture
                            context->CopyResource(stagingTexture.get(), nativeSurface.get());
                            
                            // Map the staging texture to read pixels
                            D3D11_MAPPED_SUBRESOURCE mapped;
                            hr = context->Map(stagingTexture.get(), 0, D3D11_MAP_READ, 0, &mapped);
                            
                            if (SUCCEEDED(hr))
                            {
                                // Create FrameData with actual pixel data
                                FrameData frameData;
                                frameData.width = size.Width;
                                frameData.height = size.Height;
                                frameData.stride = mapped.RowPitch;
                                frameData.data = mapped.pData;
                                frameData.size = mapped.RowPitch * size.Height;
                                frameData.timestamp = GetTickCount64();
                                
                                // Send the real screen pixels!
                                m_frameCallback(frameData);
                                
                                context->Unmap(stagingTexture.get(), 0);
                                
                                std::cout << "Extracted real pixels: " << frameData.width << "x" << frameData.height 
                                         << " stride:" << frameData.stride << " size:" << frameData.size << std::endl;
                            }
                        }
                    }
                }
            }
            catch (...)
            {
                std::cout << "Failed to extract real pixels, using fallback" << std::endl;
                
                // Fallback: send placeholder frame
                FrameData frameData;
                frameData.width = size.Width;
                frameData.height = size.Height;
                frameData.stride = size.Width * 4; // BGRA format
                frameData.size = frameData.stride * size.Height;
                frameData.timestamp = GetTickCount64();
                
                static std::vector<uint8_t> fallbackData;
                fallbackData.resize(frameData.size, 64); // Dark gray fallback
                frameData.data = fallbackData.data();
                
                m_frameCallback(frameData);
            }
        }
    }
    catch (...)
    {
        std::cout << "Exception in OnFrameArrived" << std::endl;
    }
}

void WindowsGraphicsCapture::StopCapture()
{
    if (m_session)
    {
        m_session.Close();
        m_session = nullptr;
    }
    
    if (m_framePool)
    {
        m_framePool.Close();
        m_framePool = nullptr;
    }
    
    m_captureItem = nullptr;
    m_isCapturing = false;
    std::cout << "Capture stopped" << std::endl;
}

bool WindowsGraphicsCapture::SetCaptureConfig(const CaptureConfig& config)
{
    if (m_isCapturing)
        return false;
        
    m_config = config;
    return true;
}

CaptureConfig WindowsGraphicsCapture::GetCaptureConfig() const
{
    return m_config;
}

bool WindowsGraphicsCapture::SetFrameCallback(const FrameCallback& callback)
{
    m_frameCallback = callback;
    return true;
}

bool WindowsGraphicsCapture::GetLatestFrame(FrameData& outFrame) const
{
    // Placeholder - would return the most recent captured frame
    outFrame = {};
    return false;
}

bool WindowsGraphicsCapture::SaveScreenshot(const std::string& sourceId, const std::string& filePath) const
{
    // Placeholder - would capture a single frame and save to file
    return false;
}

CaptureStatistics WindowsGraphicsCapture::GetStatistics() const
{
    return m_statistics;
}

bool WindowsGraphicsCapture::IsCursorVisible() const
{
    CURSORINFO ci = {};
    ci.cbSize = sizeof(CURSORINFO);
    if (GetCursorInfo(&ci))
    {
        return (ci.flags & CURSOR_SHOWING) != 0;
    }
    return false;
}

#endif // PLATFORM_WINDOWS