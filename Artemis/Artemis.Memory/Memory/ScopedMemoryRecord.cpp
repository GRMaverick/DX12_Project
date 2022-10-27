#include "ScopedMemoryRecord.h"
#include "MemoryGlobalTracking.h"

namespace Artemis::Memory
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
