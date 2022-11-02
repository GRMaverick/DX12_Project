#include "CommandQueueFactory.h"

#include "GraphicsDeviceFactory.h"
#include "DirectX12/CommandQueueDx12.h"

using namespace Artemis::Renderer::Interfaces;

namespace Artemis::Renderer::Device::Factories
{
	ICommandQueue* CommandQueueFactory::Construct()
	{
		return new Artemis::Renderer::Device::Dx12::CommandQueueDx12();
	}
}
