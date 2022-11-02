#ifndef __DescriptorHeap_h__
#define __DescriptorHeap_h__

#include <d3d12.h>
#include <wrl.h>

#include "Interfaces/IDescriptorHeap.h"

namespace Artemis
{
	namespace Renderer
	{
		namespace Interfaces
		{
			class IGraphicsDevice;
		}
	}
}

namespace Artemis::Renderer::Device::Dx12
{
	class DescriptorHeapDx12 final : public Interfaces::IDescriptorHeap
	{
		friend class DeviceD3D12;

	public:
		DescriptorHeapDx12( void );
		~DescriptorHeapDx12( void ) override;

		void Reset( void ) override;

		bool Initialise( const Interfaces::IGraphicsDevice* _pDevice, Interfaces::DescriptorHeapType _type, unsigned int _numDescriptors, Interfaces::DescriptorHeapFlags _flags, const wchar_t* _pDebugName ) override;

		void                        Increment( void );
		UINT                        GetFreeIndex( void ) const override;
		UINT                        GetFreeIndexAndIncrement( void ) override;
		void*                       GetDeviceObject( void ) const override;
		UINT                        GetIncrementSize( void ) const override;
		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuStartHandle( void ) const;
		D3D12_GPU_DESCRIPTOR_HANDLE GetGpuStartHandle( void ) const;

	private:
		UINT                                         m_uiIncrementSize   = 0;
		UINT                                         m_maxDescriptors    = 0;
		UINT                                         m_actualDescriptors = 0;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_pDescriptorHeap   = nullptr;
	};
}
#endif //__DescriptorHeap_h__
