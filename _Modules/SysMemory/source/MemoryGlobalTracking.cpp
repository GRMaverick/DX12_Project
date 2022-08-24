#include "../include/MemoryGlobalTracking.h"

#include <assert.h>
#include <string.h>

#include "../include/ScopedMemoryRecord.h"
#include "../include/MemoryAllocationHeader.h"

void* ::operator new(std::size_t _size)
{
	return SysMemory::MemoryGlobalTracking::Allocate(_size);
}

void* ::operator new[](std::size_t _size)
{
	return SysMemory::MemoryGlobalTracking::Allocate(_size);
}

void ::operator delete(void* _pAddress)
{
	return SysMemory::MemoryGlobalTracking::Deallocate(_pAddress);
}

void ::operator delete[](void* _pAddress)
{
	return SysMemory::MemoryGlobalTracking::Deallocate(_pAddress);
}

namespace SysMemory
{
	const char* g_ContextName[] =
	{
		"Default Heap",
		"CPU Textures",
		"GPU Textures",
		"CPU Geometry",
		"GPU Geometry",
		"GPU Resources",
		"Render Targets",
		"Memory Tracking",
	};

	MemoryContextData g_AllocationData[(unsigned int)MemoryContextCategory::eCategories];

	MemoryContextCategory					MemoryGlobalTracking::m_eCategory = MemoryContextCategory::eDefaultHeap;
	MemoryGlobalTracking::ExplicitRecords	MemoryGlobalTracking::m_vExplicitRecords = MemoryGlobalTracking::ExplicitRecords();

	void*			MemoryGlobalTracking::Allocate(std::size_t _size)
	{
		g_AllocationData[(unsigned int)m_eCategory].Allocations++;
		g_AllocationData[(unsigned int)m_eCategory].TotalAllocationSize += _size;

		g_AllocationData[(unsigned int)MemoryContextCategory::eMemoryTracking].Allocations++;
		g_AllocationData[(unsigned int)MemoryContextCategory::eMemoryTracking].TotalAllocationSize += sizeof(MemoryAllocationHeader);

		size_t szTotalAllocation = _size + sizeof(MemoryAllocationHeader);
		void* pAlloc = malloc(szTotalAllocation);
		memset(pAlloc, 0, szTotalAllocation);

		MemoryAllocationHeader* pHeader = (MemoryAllocationHeader*)pAlloc;
		pHeader->Size = _size;
		pHeader->Category = (unsigned int)m_eCategory;

		return ((char*)pAlloc) + sizeof(MemoryAllocationHeader);
	}
	
	void			MemoryGlobalTracking::Deallocate(void* _pAddress)
	{
		if (_pAddress == nullptr)
		{
			return;
		}

		MemoryAllocationHeader* pHeader = (MemoryAllocationHeader*)(((char*)_pAddress)-sizeof(MemoryAllocationHeader));

		g_AllocationData[pHeader->Category].Deallocations++;
		g_AllocationData[pHeader->Category].TotalAllocationSize -= pHeader->Size;

		g_AllocationData[(unsigned int)MemoryContextCategory::eMemoryTracking].Deallocations++;
		g_AllocationData[(unsigned int)MemoryContextCategory::eMemoryTracking].TotalAllocationSize -= sizeof(MemoryAllocationHeader);

		return free(pHeader);
	}
	
	void			MemoryGlobalTracking::RecordExplicitAllocation(MemoryContextCategory _eCategory, void* _pAddress, std::size_t _szAllocation)
	{
		ScopedMemoryRecord ctx(MemoryContextCategory::eMemoryTracking);
		m_vExplicitRecords.push_back({ _pAddress, _szAllocation, _eCategory });
		g_AllocationData[(unsigned int)_eCategory].Allocations++;
		g_AllocationData[(unsigned int)_eCategory].TotalAllocationSize += _szAllocation;
	}

	void			MemoryGlobalTracking::RecordExplicitDellocation(void* _pAddress)
	{
		for (unsigned int i = 0; i < m_vExplicitRecords.size(); ++i)
		{
			if (_pAddress == m_vExplicitRecords[i].Address)
			{
				m_vExplicitRecords.erase(m_vExplicitRecords.begin() + i);
				g_AllocationData[(unsigned int)m_vExplicitRecords[i].Category].Deallocations++;
				g_AllocationData[(unsigned int)m_vExplicitRecords[i].Category].TotalAllocationSize -= m_vExplicitRecords[i].Size;
				return;
			}
		}
	}

	void			MemoryGlobalTracking::SetContext(MemoryContextCategory _eCategory)
	{
		m_eCategory = _eCategory;
	}
	
	MemoryContextCategory MemoryGlobalTracking::GetCurrentContext(void)
	{
		return m_eCategory;
	}

	const char*		MemoryGlobalTracking::GetContextName(MemoryContextCategory _eCategory)
	{
		assert( _eCategory < MemoryContextCategory::eCategories );
		return g_ContextName[(unsigned int)_eCategory];
	}

	MemoryContextData MemoryGlobalTracking::GetContextStats(MemoryContextCategory _eCategory)
	{
		assert(_eCategory < MemoryContextCategory::eCategories);
		return g_AllocationData[(unsigned int)_eCategory];
	}
}

