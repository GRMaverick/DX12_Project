#include "Defines.h"
#include "ConstantTable.h"

#include "D3D12\Device\DeviceD3D12.h"

#include "ConstantBufferResource.h"

#include "Artemis.Utilities\Hashing.h"

using namespace SysUtilities;

namespace SysRenderer
{
	namespace D3D12
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

		bool ConstantTable::CreateConstantBuffersEntries( const ConstantBufferParameters& _params )
		{
			assert( (m_mapConstantBuffers.size() + _params.NumberBuffers) < m_uiMaxCbVs );

			for ( unsigned int cb = 0; cb < _params.NumberBuffers; ++cb )
			{
				const unsigned long ulHashName = Hashing::SimpleHash( _params.Buffers[cb].Name, strlen( _params.Buffers[cb].Name ) );

				if ( m_mapConstantBuffers.find( ulHashName ) != m_mapConstantBuffers.end() )
				{
					LogWarning( "Constant Buffer: %s already exists!", _params.Buffers[cb].Name );
					continue;
				}

				m_mapConstantBuffers[ulHashName] = _params.Buffers[cb];
			}
			return true;
		}

		ConstantBufferResource* ConstantTable::CreateConstantBuffer( const char* _pBufferName )
		{
			const unsigned long ulHashName = Hashing::SimpleHash( (char*)_pBufferName, strlen( _pBufferName ) );
			if ( m_mapConstantBuffers.find( ulHashName ) != m_mapConstantBuffers.end() )
			{
				return DeviceD3D12::Instance()->CreateConstantBufferResource( m_mapConstantBuffers[ulHashName] );
			}

			LogError( "Constant Buffer: %s - Does not exist in Hash Map", _pBufferName );
			return nullptr;
		}
	}
}
