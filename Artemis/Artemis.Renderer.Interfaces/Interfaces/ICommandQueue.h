#pragma once

#include "IGraphicsDevice.h"

namespace Artemis::Renderer::Interfaces
{
	class ICommandQueue
	{
	public:
		virtual ~ICommandQueue( void )
		{
		}

		virtual void* GetDeviceObject( void ) = 0;
		virtual bool  Initialise( const IGraphicsDevice* _pDevice, ECommandListType _type, const wchar_t* _pDebugName ) = 0;

		virtual void   SubmitToQueue( ICommandList* _pList ) = 0;
		virtual void   ExecuteCommandLists( void ) = 0;
		virtual unsigned long Signal() = 0;
        virtual void   Wait() const = 0;;
        virtual void   Flush() = 0;
	};
}
