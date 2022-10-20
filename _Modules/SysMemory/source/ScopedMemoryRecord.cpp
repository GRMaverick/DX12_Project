#include "../include/ScopedMemoryRecord.h"
#include "../include/MemoryGlobalTracking.h"

namespace SysMemory
{
	ScopedMemoryRecord::ScopedMemoryRecord( MemoryContextCategory eCategory )
	{
		m_ePreviousCategory = MemoryGlobalTracking::GetCurrentContext();
		MemoryGlobalTracking::SetContext( eCategory );
	}

	ScopedMemoryRecord::~ScopedMemoryRecord( void )
	{
		MemoryGlobalTracking::SetContext( m_ePreviousCategory );
	}
}
