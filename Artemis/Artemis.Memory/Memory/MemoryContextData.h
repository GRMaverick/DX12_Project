#ifndef __MemoryContextData_h__
#define __MemoryContextData_h__

#include <cstdlib>

namespace Artemis::Memory
{
	struct MemoryContextData
	{
		unsigned int Allocations         = 0;
		unsigned int Deallocations       = 0;
		std::size_t  TotalAllocationSize = 0;
	};
}

#endif // __MemoryContextData_h__
