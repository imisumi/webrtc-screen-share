#pragma once

#include "../IGraphicsCapture.h"

#ifdef PLATFORM_MACOS

#include <AvailabilityMacros.h>

class MacOSGraphicsCapture : public IGraphicsCapture
{
public:
    MacOSGraphicsCapture()
    {
        static_assert(false, "macOS Graphics Capture implementation is not yet available");
    }
    
    ~MacOSGraphicsCapture() override = default;

    bool Initialize() override { return false; }
    void Shutdown() override {}
    bool IsSupported() const override { return false; }
    bool IsInitialized() const override { return false; }
    std::vector<CaptureSource> GetAvailableSources() const override { return {}; }
    bool StartCapture(const std::string& sourceId) override { return false; }
    void StopCapture() override {}
    bool IsCapturing() const override { return false; }
    const char* GetPlatformName() const override { return "macOS (Not Implemented)"; }
};

#endif // PLATFORM_MACOS