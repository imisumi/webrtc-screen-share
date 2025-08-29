#pragma once

#include <memory>
#include <vector>
#include <string>
#include <string_view>
#include <functional>

struct Monitor
{
	std::string id;
	std::string name;
	int x, y;
	int width, height;
	bool isPrimary;
	float dpiScale;
};

struct Window
{
	std::string id;
	std::string title;
	std::string processName;
	int processId;
	int x, y;
	int width, height;
	bool isVisible;
	bool isMinimized;
};

struct CaptureSource
{
	std::string id;
	std::string name;
	bool isMonitor;
	int width;
	int height;
};

enum class CaptureQuality
{
	Low,
	Medium,
	High
};

struct CaptureConfig
{
	CaptureQuality quality = CaptureQuality::Medium;
	int targetFps = 30;
	bool includeCursor = true;
	bool includeBorders = true;
};

struct CaptureStatistics
{
	uint64_t framesCapture = 0;
	uint64_t framesDropped = 0;
	double averageFps = 0.0;
	double cpuUsage = 0.0;
	uint64_t memoryUsage = 0;
};

struct FrameData
{
	void* data = nullptr;
	size_t size = 0;
	int width = 0;
	int height = 0;
	int stride = 0;
	uint64_t timestamp = 0;
};

using FrameCallback = std::function<void(const FrameData& frame)>;

class IGraphicsCapture
{
public:
	virtual ~IGraphicsCapture() = default;

	virtual bool Initialize() = 0;
	virtual bool SetD3DDevice(void* d3dDevice) = 0;
	virtual void Shutdown() = 0;

	virtual bool IsSupported() const = 0;
	virtual bool IsInitialized() const = 0;

	virtual std::vector<Monitor> GetMonitors() const = 0;
	virtual std::vector<Window> GetWindows() const = 0;
	virtual std::vector<CaptureSource> GetAvailableSources() const = 0;

	virtual bool SetCaptureConfig(const CaptureConfig& config) = 0;
	virtual CaptureConfig GetCaptureConfig() const = 0;

	virtual bool StartCapture(const std::string& sourceId) = 0;
	virtual void StopCapture() = 0;
	virtual bool IsCapturing() const = 0;

	virtual bool SetFrameCallback(const FrameCallback& callback) = 0;
	virtual bool GetLatestFrame(FrameData& outFrame) const = 0;
	virtual bool SaveScreenshot(const std::string& sourceId, const std::string& filePath) const = 0;

	virtual CaptureStatistics GetStatistics() const = 0;
	virtual bool IsCursorVisible() const = 0;

	virtual std::string_view GetPlatformName() const noexcept = 0;

	static std::unique_ptr<IGraphicsCapture> Create();
	static std::string_view GetCurrentPlatform() noexcept;
};