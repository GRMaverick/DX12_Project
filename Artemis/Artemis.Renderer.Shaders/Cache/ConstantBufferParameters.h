#ifndef __ConstantBufferParameters_h__
#define __ConstantBufferParameters_h__

#include "Interfaces/IConstantBufferParameters.h"

namespace Artemis::Renderer::Shaders
{
	class ConstantBufferParameters final : public Interfaces::IConstantBufferParameters
	{
	public:
		unsigned int    NumberBuffers = 0;
		ConstantBuffer* Buffers       = nullptr;

		unsigned int   NumberTextures = 0;
		BoundResource* Textures       = nullptr;

		unsigned int   NumberSamplers = 0;
		BoundResource* Samplers       = nullptr;
	};
}

#endif // __ConstantBufferParameters_h__
