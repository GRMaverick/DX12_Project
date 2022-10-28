#include "ShaderCompilerFactory.h"

#include "DirectX12/ShaderCompilerDXC.h"
#include "DirectX12/ShaderCompilerFXC.h"

namespace Artemis::Renderer::Device::Factories
{
    Interfaces::IShaderCompiler* ShaderCompilerFactory::Construct(const bool _bIsDxc)
    {
        if (_bIsDxc)
        {
            return new Artemis::Renderer::Device::Dx12::ShaderCompilerDxc();
        }
        else
        {
            return new Artemis::Renderer::Device::Dx12::ShaderCompilerFxc();
        }
    }
}