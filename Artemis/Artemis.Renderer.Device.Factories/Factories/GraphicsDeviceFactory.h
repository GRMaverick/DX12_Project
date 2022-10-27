#pragma once

#include "Interfaces/IGraphicsDevice.h"

namespace Artemis::Renderer::Device::Factories
{
	class GraphicsDeviceFactory
	{
	public:
		static Artemis::Renderer::Interfaces::IGraphicsDevice* Construct( const bool _bDebug );
	};
}
