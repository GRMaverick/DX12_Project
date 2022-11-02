#ifndef __BufferResource_h__
#define __BufferResource_h__

#include <d3d12.h>
#include "Helpers/d3dx12.h"

#include <wrl.h>

#include "IGraphicsDevice.h"

namespace Artemis::Renderer::Interfaces
{
	class ICommandList;

	class IGpuResource
	{
	public:
		virtual ~IGpuResource( void )
		{
		}

		virtual bool UpdateValue( const char* _pValueName, const void* _pValue, const unsigned int _szValue ) const
		{
			return false;
		}
	};
}

#endif // __BufferResource_h__
