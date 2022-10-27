#ifndef __ConstantBufferParameters_h__
#define __ConstantBufferParameters_h__

namespace SysRenderer
{
	namespace D3D12
	{
		struct ConstantBufferParameters
		{
			struct Variable
			{
				char         Name[32] = {0};
				unsigned int Size     = 0;
				unsigned int Offset   = 0;
			};

			struct ConstantBuffer
			{
				char         Name[32]        = {0};
				unsigned int Size            = 0;
				unsigned int Type            = 0;
				unsigned int BindPoint       = 0;
				unsigned int NumberVariables = 0;
				Variable*    Variables       = nullptr;
			};

			struct BoundResource
			{
				char         Name[32]  = {0};
				unsigned int Type      = 0;
				unsigned int BindPoint = 0;
				unsigned int BindCount = 0;
			};

			unsigned int    NumberBuffers = 0;
			ConstantBuffer* Buffers       = nullptr;

			unsigned int   NumberTextures = 0;
			BoundResource* Textures       = nullptr;

			unsigned int   NumberSamplers = 0;
			BoundResource* Samplers       = nullptr;
		};
	}
}

#endif // __ConstantBufferParameters_h__