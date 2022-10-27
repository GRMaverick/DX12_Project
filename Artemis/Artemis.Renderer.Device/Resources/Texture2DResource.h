#ifndef __Texture2DResource_h__
#define __Texture2DResource_h__

#include <d3d12.h>
#include <wrl.h>

#include "Helpers/d3dx12.h"

#include "Interfaces/IBufferResource.h"

namespace Artemis::Renderer::Device
{
	class CommandListDx12;
	class DescriptorHeap;

	class Texture2DResource final : public Renderer::Interfaces::IBufferResource, public Renderer::Interfaces::IGpuBufferResource
	{
	public:
		Texture2DResource( const wchar_t* _pWstrFilename, const bool _bIsDds, DescriptorHeap* _pTargetSrvHeap, ID3D12Device* _pDevice, CommandListDx12* _pCmdList, const wchar_t* _pDebugName = nullptr );

		~Texture2DResource( void ) override;

		UINT32 GetHeapIndex( void ) const { return m_HeapIndex; }

	private:
		UINT m_HeapIndex = 0;

		bool CreateFromDds( const wchar_t* _pWstrFilename, ID3D12Device* _pDevice, CommandListDx12* _pCmdList );
		bool CreateFromWic( const wchar_t* _pWstrFilename, ID3D12Device* _pDevice, CommandListDx12* _pCmdList );
	};
}

#endif __Texture2DResource_h__
