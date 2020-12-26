#ifndef __MemoryTracking_h__
#define __MemoryTracking_h__

#include <cstdlib>

#include "MemoryContextData.h"
#include "MemoryContextCategory.h"

namespace SysMemory
{
	class MemoryGlobalTracking
	{
	public:
		static void*		Allocate(std::size_t _size);
		static void			Deallocate(void* _pAddress);
		static void			SetContext(MemoryContextCategory _eCategory);
		static const char*	GetContextName(MemoryContextCategory _eCategory);
		static MemoryContextData GetContextStats(MemoryContextCategory _eCategory);
		static MemoryContextCategory GetCurrentContext(void);

	private:
		static MemoryContextCategory m_eCategory;
	};
}

#endif // __MemoryTracking_h__