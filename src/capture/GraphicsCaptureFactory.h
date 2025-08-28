#pragma once

#include "IGraphicsCapture.h"
#include <memory>
#include <string_view>

class GraphicsCaptureFactory
{
public:
    static std::unique_ptr<IGraphicsCapture> Create();
    static std::string_view GetCurrentPlatform() noexcept;
};