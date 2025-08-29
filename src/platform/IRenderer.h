#pragma once

#include <memory>
#include <string_view>

struct RenderStats
{
    float deltaTime = 0.0f;
    int drawCalls = 0;
    int triangles = 0;
    float cpuTime = 0.0f;
    float gpuTime = 0.0f;
    
    // Additional stats for tracking
    uint64_t frameCount = 0;
    float frameTimeMs = 0.0f;
    float fps = 0.0f;
    int clearCalls = 0;
};

class IWindow; // Forward declaration

class IRenderer
{
public:
    virtual ~IRenderer() = default;

    virtual bool Initialize(IWindow* window) = 0;
    virtual void Shutdown() = 0;

    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;
    virtual void Present() = 0;

    virtual void Clear(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f) = 0;
    virtual void SetViewport(int x, int y, int width, int height) = 0;
    
    // Handle window resize
    virtual void OnWindowResize(int width, int height) = 0;

    virtual RenderStats GetStats() const = 0;
    virtual std::string_view GetRendererName() const noexcept = 0;
    virtual void* GetDevice() const = 0; // For sharing with capture system
    virtual void* GetDeviceContext() const = 0; // For D3D11 context sharing

    static std::unique_ptr<IRenderer> Create();
};