#ifndef __ConstantTable_h__
#define __ConstantTable_h__

#include <map>

#include "Interfaces/IConstantBufferParameters.h"

namespace Artemis::Renderer::Interfaces
{
	class IGpuResource;
	class IGraphicsDevice;
	class IDescriptorHeap;
}

namespace Artemis::Renderer::Shaders
{
	class DescriptorHeap;
	class ConstantBufferResource;

	class ConstantTable
	{
	public:
		~ConstantTable( void );

		static ConstantTable* Instance( void );

		Interfaces::IGpuResource* CreateConstantBuffer( const char* _pBufferName, Interfaces::IGraphicsDevice* _pDevice);
		bool                    CreateConstantBuffersEntries( const Renderer::Interfaces::IConstantBufferParameters* _params );

		Interfaces::IDescriptorHeap* GetDescriptorHeap( void ) const { return m_pDescHeapCbv; }

	private:
		ConstantTable( void );

		const unsigned int                                                                      m_uiMaxCbVs    = 10;
		Interfaces::IDescriptorHeap*                                                                         m_pDescHeapCbv = nullptr;
		std::map<unsigned int, Renderer::Interfaces::IConstantBufferParameters::ConstantBuffer> m_mapConstantBuffers;
	};
}

#endif // __ConstantTable_h__
