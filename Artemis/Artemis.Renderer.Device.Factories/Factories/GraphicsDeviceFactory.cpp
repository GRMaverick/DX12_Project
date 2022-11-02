#include "GraphicsDeviceFactory.h"

#include "DirectX12/DeviceDx12.h"

using namespace Artemis::Renderer::Interfaces;

namespace Artemis::Renderer::Device::Factories
{
	IGraphicsDevice* GraphicsDeviceFactory::Construct( const bool _bDebug )
	{
		return new Artemis::Renderer::Device::Dx12::DeviceDx12( _bDebug );
	}
}
