#ifndef __ScopedMemoryRecord_h__
#define __ScopedMemoryRecord_h__

#include "MemoryContextCategory.h"

namespace Artemis::Memory
{
	class ScopedMemoryRecord
	{
	public:
		ScopedMemoryRecord( MemoryContextCategory eCategory = MemoryContextCategory::EDefaultHeap );
		~ScopedMemoryRecord( void );
	private:
		MemoryContextCategory m_ePreviousCategory = MemoryContextCategory::EDefaultHeap;
	};
}

#endif //__ScopedMemoryRecord_h__
