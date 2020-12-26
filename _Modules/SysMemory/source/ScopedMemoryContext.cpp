#include "../include/ScopedMemoryContext.h"
#include "../include/MemoryGlobalTracking.h"

namespace SysMemory
{
	ScopedMemoryContext::ScopedMemoryContext(MemoryContextCategory eCategory)
	{
		m_ePreviousCategory = MemoryGlobalTracking::GetCurrentContext();
		MemoryGlobalTracking::SetContext(eCategory);
	}
	ScopedMemoryContext::~ScopedMemoryContext(void)
	{
		MemoryGlobalTracking::SetContext(m_ePreviousCategory);
	}
}