#include "RendererFactory.h"

#include "Techniques/ForwardRenderer.h"

namespace Artemis::Renderer::Techniques
{
    Artemis::Renderer::Interfaces::IRenderer* RendererFactory::Construct(RendererTechnique _technique)
    {
        switch (_technique)
        {
        case Techniques::RendererTechnique::ForwardRenderer:
            return new Artemis::Renderer::Techniques::ForwardRenderer();
        case Techniques::RendererTechnique::DeferredRenderer:
        case Techniques::RendererTechnique::ForwardPlusRenderer:
        default:
            return nullptr;
        }
    }
}