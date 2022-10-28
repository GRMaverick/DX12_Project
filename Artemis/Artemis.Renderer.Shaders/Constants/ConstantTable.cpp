#include <cassert>

#include "ConstantTable.h"

#include "Hashing/Hashing.h"

#include "Interfaces/IGraphicsDevice.h"
#include "Interfaces/IBufferResource.h"

#include "Logging/Logger.h"

#define LogWarning(pFormat, ...)		Logger::Log(SEVERITY_WARN, CATEGORY_RENDERER, pFormat, __VA_ARGS__)
#define LogError(pFormat, ...)			Logger::Log(SEVERITY_ERR, CATEGORY_RENDERER, pFormat, __VA_ARGS__)

using namespace Artemis::Utilities;

namespace Artemis::Renderer::Interfaces
{
	class IGpuResource;
	class IGraphicsDevice;
}

namespace Artemis::Renderer::Shaders
{
	ConstantTable::ConstantTable( void )
	{
	}

	ConstantTable::~ConstantTable( void )
	{
	}

	ConstantTable* ConstantTable::Instance( void )
	{
		static ConstantTable table;
		return &table;
	}

	bool ConstantTable::CreateConstantBuffersEntries( const Renderer::Interfaces::IConstantBufferParameters* _params )
	{
		assert( (m_mapConstantBuffers.size() + _params->NumberBuffers) < m_uiMaxCbVs );

		for ( unsigned int cb = 0; cb < _params->NumberBuffers; ++cb )
		{
			const unsigned long ulHashName = SimpleHash( _params->Buffers[cb].Name, strlen( _params->Buffers[cb].Name ) );

			if ( m_mapConstantBuffers.contains( ulHashName ) )
			{
				LogWarning( "Constant Buffer: %s already exists!", _params->Buffers[cb].Name );
				continue;
			}

			m_mapConstantBuffers[ulHashName] = _params->Buffers[cb];
		}
		return true;
	}

	Interfaces::IGpuResource* ConstantTable::CreateConstantBuffer( const char* _pBufferName, Interfaces::IGraphicsDevice* _pDevice )
	{
		if ( const unsigned long ulHashName = SimpleHash( const_cast<char*>(_pBufferName), strlen( _pBufferName ) ); m_mapConstantBuffers.contains( ulHashName ) )
		{
			return _pDevice->CreateConstantBufferResource( m_mapConstantBuffers[ulHashName] );
		}

		LogError( "Constant Buffer: %s - Does not exist in Hash Map", _pBufferName );
		return nullptr;
	}
}
