#pragma once

#include "Interfaces/IShaderCompiler.h"

namespace Artemis::Renderer::Device::Factories
{
    class ShaderCompilerFactory
    {
    public:
        static Interfaces::IShaderCompiler* Construct(const bool _bIsDxc);
    };
}