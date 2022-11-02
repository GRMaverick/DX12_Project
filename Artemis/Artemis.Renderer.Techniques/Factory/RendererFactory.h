#pragma once

#include "Interfaces/IRenderer.h"

namespace Artemis::Renderer::Techniques
{
    enum class RendererTechnique
    {
        ForwardRenderer,
        ForwardPlusRenderer,
        DeferredRenderer,
    };

    class RendererFactory
    {
    public:
        static Artemis::Renderer::Interfaces::IRenderer* Construct(RendererTechnique _technique);
    };
}