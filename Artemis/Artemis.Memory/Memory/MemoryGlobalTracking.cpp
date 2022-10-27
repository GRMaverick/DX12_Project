#include "MemoryGlobalTracking.h"

#include <cassert>

#include "ScopedMemoryRecord.h"
#include "MemoryAllocationHeader.h"

auto ::operator new( std::size_t _size )->void*
{
	return Artemis::Memory::MemoryGlobalTracking::Allocate( _size );
}

auto ::operator new[]( std::size_t _size )->void*
{
	return Artemis::Memory::MemoryGlobalTracking::Allocate( _size );
}

void ::operator delete( void* _pAddress )
{
	return Artemis::Memory::MemoryGlobalTracking::Deallocate( _pAddress );
}

void ::operator delete[]( void* _pAddress )
{
	return Artemis::Memory::MemoryGlobalTracking::Deallocate( _pAddress );
}

namespace Artemis::Memory
{
	const char* g_ContextName[] = {"Default Heap", "CPU Textures", "GPU Textures", "CPU Geometry", "GPU Geometry", "GPU Resources", "Render Targets", "Memory Tracking", "Shaders",};

	MemoryContextData g_AllocationData[static_cast<unsigned int>(MemoryContextCategory::ECategories)];

	MemoryContextCategory                 MemoryGlobalTracking::m_eCategory        = MemoryContextCategory::EDefaultHeap;
	MemoryGlobalTracking::ExplicitRecords MemoryGlobalTracking::m_vExplicitRecords = ExplicitRecords();

	void* MemoryGlobalTracking::Allocate( const std::size_t _size )
	{
		g_AllocationData[static_cast<unsigned int>(m_eCategory)].Allocations++;
		g_AllocationData[static_cast<unsigned int>(m_eCategory)].TotalAllocationSize += _size;

		g_AllocationData[static_cast<unsigned int>(MemoryContextCategory::EMemoryTracking)].Allocations++;
		g_AllocationData[static_cast<unsigned int>(MemoryContextCategory::EMemoryTracking)].TotalAllocationSize += sizeof( MemoryAllocationHeader );

		const size_t szTotalAllocation = _size + sizeof( MemoryAllocationHeader );
		void*        pAlloc            = malloc( szTotalAllocation );
		memset( pAlloc, 0, szTotalAllocation );

		const auto pHeader = static_cast<MemoryAllocationHeader*>(pAlloc);
		pHeader->Size      = _size;
		pHeader->Category  = static_cast<unsigned int>(m_eCategory);

		return static_cast<char*>(pAlloc) + sizeof( MemoryAllocationHeader );
	}

	void MemoryGlobalTracking::Deallocate( void* _pAddress )
	{
		if ( _pAddress == nullptr )
		{
			return;
		}

		const auto pHeader = reinterpret_cast<MemoryAllocationHeader*>(static_cast<char*>(_pAddress) - sizeof( MemoryAllocationHeader ));

		g_AllocationData[pHeader->Category].Deallocations++;
		g_AllocationData[pHeader->Category].TotalAllocationSize -= pHeader->Size;

		g_AllocationData[static_cast<unsigned int>(MemoryContextCategory::EMemoryTracking)].Deallocations++;
		g_AllocationData[static_cast<unsigned int>(MemoryContextCategory::EMemoryTracking)].TotalAllocationSize -= sizeof( MemoryAllocationHeader );

		return free( pHeader );
	}

	void MemoryGlobalTracking::RecordExplicitAllocation( MemoryContextCategory _eCategory, void* _pAddress, std::size_t _szAllocation )
	{
		ScopedMemoryRecord ctx( MemoryContextCategory::EMemoryTracking );
		m_vExplicitRecords.push_back( {_pAddress, _szAllocation, _eCategory} );
		g_AllocationData[static_cast<unsigned int>(_eCategory)].Allocations++;
		g_AllocationData[static_cast<unsigned int>(_eCategory)].TotalAllocationSize += _szAllocation;
	}

	void MemoryGlobalTracking::RecordExplicitDellocation( const void* _pAddress )
	{
		for ( unsigned int i = 0; i < m_vExplicitRecords.size(); ++i )
		{
			if ( _pAddress == m_vExplicitRecords[i].Address )
			{
				m_vExplicitRecords.erase( m_vExplicitRecords.begin() + i );
				g_AllocationData[static_cast<unsigned int>(m_vExplicitRecords[i].Category)].Deallocations++;
				g_AllocationData[static_cast<unsigned int>(m_vExplicitRecords[i].Category)].TotalAllocationSize -= m_vExplicitRecords[i].Size;
				return;
			}
		}
	}

	void MemoryGlobalTracking::SetContext( MemoryContextCategory _eCategory )
	{
		m_eCategory = _eCategory;
	}

	MemoryContextCategory MemoryGlobalTracking::GetCurrentContext( void )
	{
		return m_eCategory;
	}

	const char* MemoryGlobalTracking::GetContextName( MemoryContextCategory _eCategory )
	{
		assert( _eCategory < MemoryContextCategory::ECategories );
		return g_ContextName[static_cast<unsigned int>(_eCategory)];
	}

	MemoryContextData MemoryGlobalTracking::GetContextStats( MemoryContextCategory _eCategory )
	{
		assert( _eCategory < MemoryContextCategory::ECategories );
		return g_AllocationData[static_cast<unsigned int>(_eCategory)];
	}
}
