#ifndef __ScopedMemoryRecord_h__
#define __ScopedMemoryRecord_h__

#include "MemoryContextCategory.h"

namespace SysMemory
{
	class ScopedMemoryRecord
	{
	public:
		ScopedMemoryRecord(MemoryContextCategory eCategory = MemoryContextCategory::eDefaultHeap);
		~ScopedMemoryRecord(void);
	private:
		MemoryContextCategory m_ePreviousCategory = MemoryContextCategory::eDefaultHeap;
	};
}

#endif //__ScopedMemoryRecord_h__