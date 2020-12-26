#include "MemoryGlobalTracking.h"

#include <assert.h>
#include <string.h>

#include "../include/ScopedMemoryContext.h"
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
		"GPU Textures",
		"CPU Geometry",
		"GPU Geometry",
		"GPU Resources",
		"Render Targets",
		"Memory Tracking",
	};

	MemoryContextData g_AllocationData[(unsigned int)MemoryContextCategory::eCategories];

	MemoryContextCategory MemoryGlobalTracking::m_eCategory = MemoryContextCategory::eDefaultHeap;

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

