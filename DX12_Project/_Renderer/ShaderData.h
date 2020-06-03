#ifndef __ShaderData_h__
#define __ShaderData_h__

struct ShaderData
{
public:
	ShaderData(void) { }
	~ShaderData(void) { }

	char			ShaderName[50];
	size_t			ShaderByteCodeSize;
	char* ShaderByteCode;
};

#endif // __ShaderData_h__