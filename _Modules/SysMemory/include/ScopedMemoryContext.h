#ifndef __ScopedMemoryContext_h__
#define __ScopedMemoryContext_h__

#include "MemoryContextCategory.h"

namespace SysMemory
{
	class ScopedMemoryContext
	{
	public:
		ScopedMemoryContext(MemoryContextCategory eCategory = MemoryContextCategory::eDefaultHeap);
		~ScopedMemoryContext(void);
	private:
		MemoryContextCategory m_ePreviousCategory = MemoryContextCategory::eDefaultHeap;
	};
}

#endif //__ScopedMemoryContext_h__