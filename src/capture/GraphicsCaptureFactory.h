#pragma once

#include "IGraphicsCapture.h"
#include <memory>

class GraphicsCaptureFactory
{
public:
    static std::unique_ptr<IGraphicsCapture> Create();
    static const char* GetCurrentPlatform();
};