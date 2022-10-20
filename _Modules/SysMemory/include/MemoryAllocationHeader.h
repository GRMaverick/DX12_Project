#ifndef __MemoryAllocationHeader_h__
#define __MemoryAllocationHeader_h__

#include <cstdlib>

namespace SysMemory
{
	struct MemoryAllocationHeader
	{
		std::size_t  Size     = 0;
		unsigned int Category = 0;
	};
}

#endif // __MemoryAllocationHeader_h__
