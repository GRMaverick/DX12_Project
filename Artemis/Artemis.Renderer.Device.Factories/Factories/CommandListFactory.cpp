#include "CommandListFactory.h"

#include "CommandListFactory.h"
#include "DirectX12/CommandListDx12.h"

using namespace Artemis::Renderer::Interfaces;

namespace Artemis::Renderer::Device::Factories
{
    ICommandList* CommandListFactory::Construct()
    {
        return new Artemis::Renderer::Device::Dx12::CommandListDx12();
    }
}
