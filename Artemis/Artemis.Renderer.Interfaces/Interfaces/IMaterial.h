#pragma once

namespace Artemis::Renderer::Interfaces
{
    class IShaderStage;

    struct IMaterial
    {
        IShaderStage* m_pVertexShader;
        IShaderStage* m_pPixelShader;
    };
}