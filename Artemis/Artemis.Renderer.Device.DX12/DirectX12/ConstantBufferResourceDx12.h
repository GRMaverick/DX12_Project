#ifndef __ConstantBufferResource_h__
#define __ConstantBufferResource_h__

#include "GpuResourceDx12.h"

#include "Interfaces/IConstantBufferParameters.h"

namespace Artemis::Renderer::Device::Dx12
{
	class DescriptorHeap;

	class ConstantBufferResourceDx12 final : public GpuResourceDx12
	{
	public:
		ConstantBufferResourceDx12( const Interfaces::IGraphicsDevice* _pDevice, Interfaces::IDescriptorHeap* _pDescHeapCbv, const Interfaces::IConstantBufferParameters::ConstantBuffer& _params, const wchar_t* _pName );
		~ConstantBufferResourceDx12( void ) override;

		virtual bool UpdateValue( const char* _pValueName, const void* _pValue, unsigned int _szValue ) const override final;

		unsigned int GetBindPoint( void ) const { return m_cbpConstantParameters.BindPoint; }
	private:
		char*                                                 m_pCpuMapped = nullptr;
		Interfaces::IConstantBufferParameters::ConstantBuffer m_cbpConstantParameters;
	};
}

#endif // __ConstantBufferResource_h__
