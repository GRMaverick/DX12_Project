#include "Defines.h"

#include "Texture2DResource.h"

Texture2DResource::Texture2DResource(void)
{

}
Texture2DResource::~Texture2DResource(void)
{

}

bool Texture2DResource::Initialise(UINT32 _heapIndex)
{
	m_HeapIndex = _heapIndex;

	return true;
}