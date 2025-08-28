#pragma once

#include "../IGraphicsCapture.h"
#include <windows.h>
#include <winrt/base.h>
#include <winrt/Windows.Graphics.Capture.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.Foundation.h>

class WindowsGraphicsCapture : public IGraphicsCapture
{
public:
    WindowsGraphicsCapture();
    ~WindowsGraphicsCapture() override;

    bool Initialize() override;
    bool SetD3DDevice(void* d3dDevice) override;
    void Shutdown() override;

    bool IsSupported() const override;
    bool IsInitialized() const override { return m_initialized; }

    std::vector<Monitor> GetMonitors() const override;
    std::vector<Window> GetWindows() const override;
    std::vector<CaptureSource> GetAvailableSources() const override;

    bool SetCaptureConfig(const CaptureConfig& config) override;
    CaptureConfig GetCaptureConfig() const override;

    bool StartCapture(const std::string& sourceId) override;
    void StopCapture() override;
    bool IsCapturing() const override { return m_isCapturing; }

    bool SetFrameCallback(const FrameCallback& callback) override;
    bool GetLatestFrame(FrameData& outFrame) const override;
    bool SaveScreenshot(const std::string& sourceId, const std::string& filePath) const override;

    CaptureStatistics GetStatistics() const override;
    bool IsCursorVisible() const override;

    std::string_view GetPlatformName() const noexcept override { return "Windows Graphics Capture API"; }

private:
    bool m_initialized = false;
    bool m_isCapturing = false;
    CaptureConfig m_config;
    FrameCallback m_frameCallback;
    CaptureStatistics m_statistics;
    
    winrt::Windows::Graphics::Capture::GraphicsCaptureSession m_session{ nullptr };
    winrt::Windows::Graphics::Capture::GraphicsCaptureItem m_captureItem{ nullptr };
    winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool m_framePool{ nullptr };
    winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice m_device{ nullptr };

    void OnFrameArrived();
};