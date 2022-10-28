#pragma once

#include "Interfaces/IGraphicsDevice.h"
#include "Interfaces/ICommandList.h"

namespace Artemis::Renderer::Device::Factories
{
    class CommandListFactory
    {
    public:
        static Artemis::Renderer::Interfaces::ICommandList* Construct();
    };
}
