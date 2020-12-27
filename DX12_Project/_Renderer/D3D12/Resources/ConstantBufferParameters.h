#ifndef __ConstantBufferParameters_h__
#define __ConstantBufferParameters_h__

struct ConstantBufferParameters
{
	struct Variable
	{
		char			Name[32] = { 0 };
		unsigned int	Size = 0;
		unsigned int	Offset = 0;
	};

	struct ConstantBuffer
	{
		char			Name[32] = { 0 };
		unsigned int	Size = 0;
		unsigned int	Type = 0;
		unsigned int	NumberVariables = 0;
		Variable* Variables = nullptr;
	};

	struct BoundResource
	{
		char			Name[32] = { 0 };
		unsigned int	Type = 0;
		unsigned int	BindPoint = 0;
		unsigned int	BindCount = 0;
	};

	unsigned int		NumberBuffers = 0;
	ConstantBuffer* Buffers = nullptr;

	unsigned int		NumberResources = 0;
	BoundResource* Resources = nullptr;
};

#endif // __ConstantBufferParameters_h__