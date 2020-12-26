#ifndef __MemoryContextCategory_h__
#define __MemoryContextCategory_h__

namespace SysMemory
{
	enum class MemoryContextCategory : unsigned int
	{
		eDefaultHeap = 0,
		eTextureCPU,
		eTextureGPU,
		eGeometryCPU,
		eGeometryGPU,
		eResourceGPU,
		eRenderTarget,
		eMemoryTracking,
		eCategories,
	};
}

#endif // __MemoryContextCategory_h__