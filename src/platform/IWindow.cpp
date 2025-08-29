#include "IWindow.h"

#include <cassert>

#ifdef PLATFORM_WINDOWS
#include "windows/Win32Window.h"
#endif

std::unique_ptr<IWindow> IWindow::Create(const WindowConfig& config)
{
#ifdef PLATFORM_WINDOWS
	return std::make_unique<Win32Window>(config);
#elif defined(PLATFORM_MACOS)
	static_assert(false, "IWindow::Create() not implemented for macOS yet");
#elif defined(PLATFORM_LINUX)
	static_assert(false, "IWindow::Create() not implemented for Linux yet");
#else
	static_assert(false, "IWindow::Create() not implemented for this platform yet");
	return nullptr;
#endif
}

