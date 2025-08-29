#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <functional>

#include "IRenderer.h"

struct WindowConfig
{
	std::string title = "Application";
	int width = 1280;
	int height = 720;
	bool resizable = true;
	bool vsync = true;
};

enum class KeyCode
{
	Unknown = 0,
	Escape,
	Space,
	Enter,
	A,
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	I,
	J,
	K,
	L,
	M,
	N,
	O,
	P,
	Q,
	R,
	S,
	T,
	U,
	V,
	W,
	X,
	Y,
	Z,
	// Add more as needed
};

enum class MouseButton
{
	Left = 0,
	Right = 1,
	Middle = 2
};

struct WindowEvent
{
	enum Type
	{
		Resize,
		Close,
		KeyPress,
		KeyRelease,
		MousePress,
		MouseRelease,
		MouseMove
	};
	Type type;

	// Event data
	union
	{
		struct
		{
			int width, height;
		} resize;
		struct
		{
			KeyCode key;
		} keyboard;
		struct
		{
			MouseButton button;
			int x, y;
		} mouse;
		struct
		{
			int x, y;
		} mouseMove;
	};
};

enum class WindowAPI
{
	Auto,	// Choose best for platform
	Win32,	// Windows native
	Cocoa,	// macOS native
	X11,	// Linux X11
	Wayland // Linux Wayland
};

enum class RendererAPI
{
	Auto,	   // Choose best for platform
	DirectX11, // Windows D3D11
	DirectX12, // Windows D3D12
	OpenGL,	   // Cross-platform OpenGL
	Vulkan,	   // Cross-platform Vulkan
	Metal	   // macOS/iOS Metal
};

using WindowEventCallback = std::function<void(const WindowEvent &)>;

class IWindow
{
public:
	virtual ~IWindow() = default;

	virtual void Shutdown() = 0;

	virtual void PollEvents() = 0;
	virtual bool ShouldClose() const = 0;

	virtual void SetEventCallback(const WindowEventCallback &callback) = 0;

	virtual void SetTitle(const std::string &title) = 0;
	virtual void SetSize(int width, int height) = 0;
	virtual void GetSize(int &width, int &height) const = 0;

	virtual void *GetNativeHandle() const = 0; // HWND on Windows, etc.

	virtual std::string_view GetPlatformName() const noexcept = 0;

	static std::unique_ptr<IWindow> Create(const WindowConfig &config);
};