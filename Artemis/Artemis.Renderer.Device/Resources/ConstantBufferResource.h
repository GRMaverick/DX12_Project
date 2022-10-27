#ifndef __ConstantBufferResource_h__
#define __ConstantBufferResource_h__

#include "Interfaces/IBufferResource.h"

#include "Interfaces/IConstantBufferParameters.h"

namespace Artemis::Renderer::Device
{
	class DescriptorHeap;

	class ConstantBufferResource final : public Interfaces::IBufferResource, public Interfaces::IGpuBufferResource
	{
	public:
		ConstantBufferResource( ID3D12Device* _pDevice, DescriptorHeap* _pDescHeapCbv, const Interfaces::IConstantBufferParameters::ConstantBuffer& _params, const wchar_t* _pName );
		~ConstantBufferResource( void ) override;

		bool UpdateValue( const char* _pValueName, const void* _pValue, unsigned int _szValue ) const;

		unsigned int GetHeapIndex( void ) const { return m_uiHeapIndex; }
		unsigned int GetBindPoint( void ) const { return m_cbpConstantParameters.BindPoint; }
	private:
		unsigned int                                          m_uiHeapIndex;
		char*                                                 m_pCpuMapped = nullptr;
		Interfaces::IConstantBufferParameters::ConstantBuffer m_cbpConstantParameters;
	};
}

#endif // __ConstantBufferResource_h__
