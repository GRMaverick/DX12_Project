#ifndef __MemoryContextCategory_h__
#define __MemoryContextCategory_h__

namespace Artemis::Memory
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
		EShader,
		ECategories,
	};
}

#endif // __MemoryContextCategory_h__
