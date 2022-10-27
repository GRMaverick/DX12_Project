#ifndef __ConstantTable_h__
#define __ConstantTable_h__

#include <map>

#include "Interfaces/IConstantBufferParameters.h"

namespace Artemis::Renderer::Device
{
	class DescriptorHeap;
	class ConstantBufferResource;

	class ConstantTable
	{
	public:
		~ConstantTable( void );

		static ConstantTable* Instance( void );

		ConstantBufferResource* CreateConstantBuffer( const char* _pBufferName );
		bool                    CreateConstantBuffersEntries( const Renderer::Interfaces::IConstantBufferParameters* _params );

		DescriptorHeap* GetDescriptorHeap( void ) const { return m_pDescHeapCbv; }

	private:
		ConstantTable( void );

		const unsigned int                                                                      m_uiMaxCbVs    = 10;
		DescriptorHeap*                                                                         m_pDescHeapCbv = nullptr;
		std::map<unsigned int, Renderer::Interfaces::IConstantBufferParameters::ConstantBuffer> m_mapConstantBuffers;
	};
}

#endif // __ConstantTable_h__
