#pragma once

#include "Interfaces/IGraphicsDevice.h"
#include "Interfaces/ICommandQueue.h"

namespace Artemis::Renderer::Device::Factories
{
	class CommandQueueFactory
	{
	public:
		static Artemis::Renderer::Interfaces::ICommandQueue* Construct();
	};
}
