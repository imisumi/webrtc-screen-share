#include "IRenderer.h"

#include <cassert>

#ifdef PLATFORM_WINDOWS
#include "windows/D3D11Renderer.h"
#endif

std::unique_ptr<IRenderer> IRenderer::Create()
{
#ifdef PLATFORM_WINDOWS
	return std::make_unique<D3D11Renderer>();
#elif defined(PLATFORM_MACOS)
	static_assert(false, "IRenderer::Create() not implemented for macOS yet");
#elif defined(PLATFORM_LINUX)
	static_assert(false, "IRenderer::Create() not implemented for Linux yet");
#else
	static_assert(false, "IRenderer::Create() not implemented for this platform yet");
	return nullptr;
#endif
}