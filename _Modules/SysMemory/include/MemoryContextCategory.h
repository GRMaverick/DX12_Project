#ifndef __MemoryContextCategory_h__
#define __MemoryContextCategory_h__

namespace SysMemory
{
	enum class MemoryContextCategory : unsigned int
	{
		EDefaultHeap = 0,
		ETextureCpu,
		ETextureGpu,
		EGeometryCpu,
		EGeometryGpu,
		EResourceGpu,
		ERenderTarget,
		EMemoryTracking,
		ECategories,
	};
}

#endif // __MemoryContextCategory_h__
