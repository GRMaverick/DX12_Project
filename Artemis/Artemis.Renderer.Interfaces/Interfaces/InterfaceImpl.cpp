#include <assert.h>

#include "IConstantBufferParameters.h"
#include "IRenderer.h"
#include "IShader.h"
#include "IShaderCompiler.h"
#include "ISamplerState.h"

#include "ICommandList.h"
#include "IDescriptorHeap.h"

#include "IBufferResource.h"
#include "IGraphicsDevice.h"

#include "Memory/MemoryGlobalTracking.h"

namespace Artemis::Renderer::Interfaces
{
	ISamplerState::~ISamplerState( void )
	{
	}

	IShaderStage::~IShaderStage( void )
	{
	}

	IShaderCompiler::~IShaderCompiler( void )
	{
	}

	IConstantBufferParameters::~IConstantBufferParameters( void )
	{
	}

	ICommandList::~ICommandList( void )
	{
	}
}
