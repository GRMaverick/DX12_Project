#ifndef __Texture2DResource_h__
#define __Texture2DResource_h__

#include <d3d12.h>

#include "Helpers/d3dx12.h"

#include "GpuResourceDx12.h"

namespace Artemis::Renderer::Interfaces
{
	class IGraphicsDevice;
	class ICommandList;
	class IDescriptorHeap;
}

namespace Artemis::Renderer::Device::Dx12
{
	class CommandListDx12;

	class Texture2DResourceDx12 final : public GpuResourceDx12
	{
	public:
		Texture2DResourceDx12( const wchar_t* _pWstrFilename, const bool _bIsDds, const Interfaces::IGraphicsDevice* _pDevice, Interfaces::ICommandList* _pCmdList, Interfaces::IDescriptorHeap* _pTargetSrvHeap, const wchar_t* _pDebugName = nullptr );
		~Texture2DResourceDx12( void ) override;

	private:
		bool CreateFromDds( const wchar_t* _pWstrFilename, ID3D12Device* _pDevice, CommandListDx12* _pCmdList );
		bool CreateFromWic( const wchar_t* _pWstrFilename, ID3D12Device* _pDevice, CommandListDx12* _pCmdList );
	};
}

#endif __Texture2DResource_h__
