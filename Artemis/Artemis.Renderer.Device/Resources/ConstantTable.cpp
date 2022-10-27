#include <cassert>

#include "DeviceDefines.h"
#include "ConstantTable.h"

#include "Device/DeviceD3D12.h"

#include "ConstantBufferResource.h"

#include "Hashing/Hashing.h"

using namespace Artemis::Utilities;

namespace Artemis::Renderer::Device
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

	ConstantBufferResource* ConstantTable::CreateConstantBuffer( const char* _pBufferName )
	{
		if ( const unsigned long ulHashName = SimpleHash( const_cast<char*>(_pBufferName), strlen( _pBufferName ) ); m_mapConstantBuffers.contains( ulHashName ) )
		{
			return DeviceD3D12::Instance()->CreateConstantBufferResource( m_mapConstantBuffers[ulHashName] );
		}

		LogError( "Constant Buffer: %s - Does not exist in Hash Map", _pBufferName );
		return nullptr;
	}
}
