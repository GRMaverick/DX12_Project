#ifndef __ConstantTable_h__
#define __ConstantTable_h__

#include <map>

#include "ConstantBufferParameters.h"

namespace SysRenderer
{
	namespace D3D12
	{
		class DescriptorHeap;
		class ConstantBufferResource;

		class ConstantTable
		{
		public:
			~ConstantTable(void);

			static ConstantTable* Instance(void);

			ConstantBufferResource* CreateConstantBuffer(const char* _pBufferName);
			bool CreateConstantBuffersEntries(const ConstantBufferParameters& _params);

			DescriptorHeap* GetDescriptorHeap(void) { return m_pDescHeapCBV; }

		private:
			ConstantTable(void);

			const unsigned int kMaxCBVs = 10;
			DescriptorHeap* m_pDescHeapCBV = nullptr;
			std::map<unsigned int, ConstantBufferParameters::ConstantBuffer> m_mapConstantBuffers;
		};
	}
}

#endif // __ConstantTable_h__