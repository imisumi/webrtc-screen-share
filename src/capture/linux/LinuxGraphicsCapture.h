#pragma once

#include "../IGraphicsCapture.h"

#ifdef PLATFORM_LINUX

class LinuxGraphicsCapture : public IGraphicsCapture
{
public:
    LinuxGraphicsCapture()
    {
        static_assert(false, "Linux Graphics Capture implementation is not yet available");
    }
    
    ~LinuxGraphicsCapture() override = default;

    bool Initialize() override { return false; }
    void Shutdown() override {}
    bool IsSupported() const override { return false; }
    bool IsInitialized() const override { return false; }
    std::vector<CaptureSource> GetAvailableSources() const override { return {}; }
    bool StartCapture(const std::string& sourceId) override { return false; }
    void StopCapture() override {}
    bool IsCapturing() const override { return false; }
    const char* GetPlatformName() const override { return "Linux (Not Implemented)"; }
};

#endif // PLATFORM_LINUX