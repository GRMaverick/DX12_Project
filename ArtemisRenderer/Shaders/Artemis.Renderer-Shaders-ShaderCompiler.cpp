module;

#include "ShaderCompilerDXC.h"
#include "ShaderCompilerFXC.h"

#include <Windows.h>
#include <assert.h>
#include <stdlib.h>
#include <dxcapi.h>
#include <d3dcompiler.h>
#include <vector>

#include "Shader.h"

#include "../Helpers/Defines.h"

#include "../Resources/ConstantBufferParameters.h"

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxcompiler.lib")

module Artemis.Renderer:Shaders;

//#define DUMP_CONSTANTS
//#define DUMP_BLOBS

namespace ArtemisRenderer::Shaders
{
	using namespace Resources;

	ShaderCompilerFXC::ShaderCompilerFXC(void)
	{

	}

	ShaderCompilerFXC::~ShaderCompilerFXC(void)
	{

	}

	const char* GetTargetProfileA(const char* _pFilename)
	{
		if (strstr(_pFilename, "MainVS"))
		{
			// Vertex Shader
			return "vs_5_1";
		}
		else if (strstr(_pFilename, "MainPS"))
		{
			// Pixel Shader
			return "ps_5_1";
		}
		return "";
	}		
	
	const wchar_t* GetTargetProfileW(const char* _pFilename)
	{
		if (strstr(_pFilename, "MainVS"))
		{
			// Vertex Shader
			return L"vs_6_0";
		}
		else if (strstr(_pFilename, "MainPS"))
		{
			// Pixel Shader
			return L"ps_6_0";
		}
		return L"";
	}

	IShaderStage* ShaderCompilerDXC::Compile(const char* _pFilename, const char* _pFunctionName, char* _pError)
	{
		IDxcLibrary* pLibrary = nullptr;
		VALIDATE_D3D(DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&pLibrary)));

		size_t numRead = 0;
		wchar_t* pFilenameWstr = new wchar_t[strlen(_pFilename)];
		mbstowcs_s(&numRead, pFilenameWstr, strlen(_pFilename) + 1, _pFilename, strlen(_pFilename));

		wchar_t* pFunctionNameWstr = new wchar_t[strlen(_pFunctionName)];
		mbstowcs_s(&numRead, pFunctionNameWstr, strlen(_pFunctionName) + 1, _pFunctionName, strlen(_pFunctionName));

		IDxcBlobEncoding* pBlob = nullptr;
		VALIDATE_D3D(pLibrary->CreateBlobFromFile(pFilenameWstr, 0, &pBlob));

		IDxcCompiler* pCompiler = nullptr;
		VALIDATE_D3D(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&pCompiler)));

		const wchar_t* targetProfile = GetTargetProfileW(_pFunctionName);

		LPCWSTR args[] =
		{
			L"/Zi",
			L"/Od",
		};

		if (!pBlob)
		{
			LogError("%s failed to load", _pFilename);
			return nullptr;
		}

		IDxcIncludeHandler* pIncludeHandler = nullptr;
		pLibrary->CreateIncludeHandler(&pIncludeHandler);

		IDxcOperationResult* pOpsResult = nullptr;
		VALIDATE_D3D(pCompiler->Compile(pBlob, pFilenameWstr, pFunctionNameWstr, targetProfile, args, _countof(args), nullptr, 0, pIncludeHandler, &pOpsResult));

		pIncludeHandler->Release();

		if (!pOpsResult)
		{
			LogError("%s failed to compile", _pFilename);
			return nullptr;
		}

		HRESULT status = S_OK;
		VALIDATE_D3D(pOpsResult->GetStatus(&status));
		if (FAILED(status))
		{
			IDxcBlobEncoding* pError = nullptr;
			pOpsResult->GetErrorBuffer(&pError);

			const char* pErrorString = (const char*)pError->GetBufferPointer();
			LogError("Shader Compiler Error %s\n", pErrorString);
			pError->Release();
			return nullptr;
		}

		IDxcBlob* pCompiledCode = nullptr;
		VALIDATE_D3D(pOpsResult->GetResult(&pCompiledCode));

		if (!pCompiledCode)
		{
			return nullptr;
		}

		IShaderStage* pShader = new Shader(pCompiledCode->GetBufferPointer(), pCompiledCode->GetBufferSize(),
			targetProfile[0] == L'v' ? IShaderStage::ShaderType::VertexShader : IShaderStage::ShaderType::PixelShader
		);

		pCompiledCode->Release();
		pOpsResult->Release();
		pCompiler->Release();
		pBlob->Release();
		pLibrary->Release();

		return pShader;
	}
	IShaderStage* ShaderCompilerFXC::Compile(const char* _pFilename, const char* _pFunctionName, char* _pError)
	{
		ID3DBlob* pError = nullptr;
		ID3DBlob* pByteCode = nullptr;

		size_t numRead = 0;
		wchar_t* pFilenameWstr = new wchar_t[strlen(_pFilename)];
		mbstowcs_s(&numRead, pFilenameWstr, strlen(_pFilename) + 1, _pFilename, strlen(_pFilename));

		const char* pStrTargetProfile = GetTargetProfileA(_pFunctionName);

		UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
		VALIDATE_D3D(D3DCompileFromFile(pFilenameWstr, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, _pFunctionName,
			pStrTargetProfile, compileFlags, 0, &pByteCode, &pError));

		if (pError)
		{
			_pError = new char[pError->GetBufferSize()];
			strncpy_s(_pError, pError->GetBufferSize(), (const char*)pError->GetBufferPointer(), pError->GetBufferSize());
			pError->Release();

			LogError("\nD3DCompiler Error: %s", _pError);
			return nullptr;
		}

		IShaderStage* pShader = new Shader(pByteCode->GetBufferPointer(), pByteCode->GetBufferSize(),
			pStrTargetProfile[0] == 'v' ? IShaderStage::ShaderType::VertexShader : IShaderStage::ShaderType::PixelShader
		);

		pByteCode->Release();

		return pShader;
	}

	void ShaderCompilerDXC::Reflect(IShaderStage* _pShader)
	{
		IDxcLibrary* pLibrary = nullptr;
		VALIDATE_D3D(DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&pLibrary)));

		IDxcBlobEncoding* pBlob = nullptr;
		VALIDATE_D3D(pLibrary->CreateBlobWithEncodingOnHeapCopy(_pShader->GetBytecode(), (UINT32)_pShader->GetBytecodeSize(), CP_ACP, &pBlob));

		IDxcContainerReflection* pReflectionCntr = nullptr;
		VALIDATE_D3D(DxcCreateInstance(CLSID_DxcContainerReflection, IID_PPV_ARGS(&pReflectionCntr)));
		VALIDATE_D3D(pReflectionCntr->Load(pBlob));

#if defined(DUMP_BLOBS)
		UINT32 parts = 0;
		pReflectionCntr->GetPartCount(&parts);
		for (unsigned int part = 0; part < parts; ++part)
		{
			UINT32 kind = 0;
			VALIDATE_D3D(pReflectionCntr->GetPartKind(part, &kind));

			UINT32 shaderIdx;
			VALIDATE_D3D(pReflectionCntr->FindFirstPartKind(kind, &shaderIdx));

			IDxcBlob* pContent;
			VALIDATE_D3D(pReflectionCntr->GetPartContent(shaderIdx, &pContent));

			char CC4[5];
			CC4[0] = (char)(kind >> 0);
			CC4[1] = (char)(kind >> 8);
			CC4[2] = (char)(kind >> 16);
			CC4[3] = (char)(kind >> 24);
			CC4[4] = '\0';

			if (pContent)
			{
				LogInfo("%s: %u bytes", CC4, pContent->GetBufferSize());
				pContent->Release();
			}
		}
#endif

		UINT32 shaderIdx;
		VALIDATE_D3D(pReflectionCntr->FindFirstPartKind(DFCC_DXIL, &shaderIdx));

		ID3D12ShaderReflection* pReflection = nullptr;
		VALIDATE_D3D(pReflectionCntr->GetPartReflection(shaderIdx, IID_PPV_ARGS(&pReflection)));

		ReflectInternal(_pShader, pReflection);
	}
	void ShaderCompilerFXC::Reflect(IShaderStage* _pShader)
	{
		ID3D12ShaderReflection* pShaderReflection = nullptr;
		VALIDATE_D3D(D3DReflect(_pShader->GetBytecode(), _pShader->GetBytecodeSize(), IID_ID3D12ShaderReflection, (void**)&pShaderReflection));

		ReflectInternal(_pShader, pShaderReflection);
	}

	void ShaderCompilerDX::ReflectInternal(IShaderStage* _pShader, ID3D12ShaderReflection* _pReflection)
	{
		if (_pReflection)
		{
			D3D12_SHADER_DESC desc{};
			VALIDATE_D3D(_pReflection->GetDesc(&desc));

			ShaderIOParameters shaderIO;
			shaderIO.NumberInputs = desc.InputParameters;
			shaderIO.Inputs = new ShaderIOParameters::Parameter[shaderIO.NumberInputs];
			for (unsigned int i = 0; i < shaderIO.NumberInputs; ++i)
			{
				D3D12_SIGNATURE_PARAMETER_DESC paramDesc{};
				VALIDATE_D3D(_pReflection->GetInputParameterDesc(i, &paramDesc));

				ShaderIOParameters::Parameter& p = shaderIO.Inputs[i];
				p.Register = paramDesc.Register;
				p.ComponentType = paramDesc.ComponentType;
				p.SemanticIndex = paramDesc.SemanticIndex;
				p.SystemValueType = paramDesc.SystemValueType;
				p.Mask = paramDesc.Mask;
				strncpy_s(p.SemanticName, paramDesc.SemanticName, ARRAYSIZE(p.SemanticName));
			}

			shaderIO.NumberOutputs = desc.OutputParameters;
			shaderIO.Outputs = new ShaderIOParameters::Parameter[shaderIO.NumberOutputs];
			for (unsigned int i = 0; i < shaderIO.NumberOutputs; ++i)
			{
				D3D12_SIGNATURE_PARAMETER_DESC paramDesc{};
				VALIDATE_D3D(_pReflection->GetOutputParameterDesc(i, &paramDesc));

				ShaderIOParameters::Parameter& p = shaderIO.Outputs[i];
				p.Register = paramDesc.Register;
				p.ComponentType = paramDesc.ComponentType;
				p.SemanticIndex = paramDesc.SemanticIndex;
				p.SystemValueType = paramDesc.SystemValueType;
				p.Mask = paramDesc.Mask;
				strncpy_s(p.SemanticName, paramDesc.SemanticName, ARRAYSIZE(p.SemanticName));
			}

			ConstantBufferParameters cbInfo;
			cbInfo.NumberBuffers = desc.ConstantBuffers;
			if (cbInfo.NumberBuffers)
			{
				cbInfo.Buffers = new ConstantBufferParameters::ConstantBuffer[cbInfo.NumberBuffers];
				for (unsigned int cb = 0; cb < cbInfo.NumberBuffers; ++cb)
				{
					ID3D12ShaderReflectionConstantBuffer* pCBuffer = _pReflection->GetConstantBufferByIndex(cb);
					if (pCBuffer)
					{
						D3D12_SHADER_BUFFER_DESC sbDesc;
						VALIDATE_D3D(pCBuffer->GetDesc(&sbDesc));

#if defined(DUMP_CONSTANTS)
						LogInfo("CB Name: %s", sbDesc.Name);
						LogInfo("\tSize: %u", sbDesc.Size);
						LogInfo("\tType: %u", sbDesc.Type);
						LogInfo("\tVariables: %u", sbDesc.Variables);
#endif

						ConstantBufferParameters::ConstantBuffer& rCBuffer = cbInfo.Buffers[cb];
						strncpy_s(rCBuffer.Name, sbDesc.Name, ARRAYSIZE(rCBuffer.Name));
						rCBuffer.Size = sbDesc.Size;
						rCBuffer.Type = sbDesc.Type;
						rCBuffer.NumberVariables = sbDesc.Variables;

						if (rCBuffer.NumberVariables)
						{
							rCBuffer.Variables = new ConstantBufferParameters::Variable[rCBuffer.NumberVariables];
							for (unsigned int var = 0; var < sbDesc.Variables; ++var)
							{
								ID3D12ShaderReflectionVariable* pCBVariable = pCBuffer->GetVariableByIndex(var);
								if (pCBVariable)
								{
									D3D12_SHADER_VARIABLE_DESC varDesc;
									VALIDATE_D3D(pCBVariable->GetDesc(&varDesc));

									ID3D12ShaderReflectionType* pType = pCBVariable->GetType();
									D3D12_SHADER_TYPE_DESC typeDesc;
									VALIDATE_D3D(pType->GetDesc(&typeDesc));

									ConstantBufferParameters::Variable& rVariable = rCBuffer.Variables[var];
									strncpy_s(rVariable.Name, varDesc.Name, ARRAYSIZE(rVariable.Name));
									rVariable.Size = varDesc.Size;
									rVariable.Offset = varDesc.StartOffset;

#if defined(DUMP_CONSTANTS)
									LogInfo("\tVar Name: %s", rVariable.Name);
									LogInfo("\t\tSize: %u", rVariable.Size);
									LogInfo("\t\tOffset: %u", rVariable.Offset);
#endif
								}
							}
						}
					}
				}
			}

			if (desc.BoundResources)
			{
				std::vector<ConstantBufferParameters::BoundResource> textures;
				std::vector<ConstantBufferParameters::BoundResource> samplers;

				for (unsigned int br = 0; br < desc.BoundResources; ++br)
				{
					D3D12_SHADER_INPUT_BIND_DESC ibDesc;
					VALIDATE_D3D(_pReflection->GetResourceBindingDesc(br, &ibDesc));

					ConstantBufferParameters::BoundResource resource;
					strncpy_s(resource.Name, ibDesc.Name, ARRAYSIZE(resource.Name));
					resource.Type = ibDesc.Type;
					resource.BindPoint = ibDesc.BindPoint;
					resource.BindCount = ibDesc.BindCount;

					switch (resource.Type)
					{
					case D3D_SIT_CBUFFER:
						for (unsigned int cb = 0; cb < cbInfo.NumberBuffers; ++cb)
						{
							if (strncmp(cbInfo.Buffers[cb].Name, resource.Name, ARRAYSIZE(cbInfo.Buffers[cb].Name)) == 0)
							{
								cbInfo.Buffers[cb].BindPoint = resource.BindPoint;
							}
						}
						break;
					case D3D_SIT_TEXTURE:
						textures.push_back(resource);
						break;
					case D3D_SIT_SAMPLER:
						samplers.push_back(resource);
						break;
					}
				}

				cbInfo.NumberTextures = (unsigned int)textures.size();
				cbInfo.Textures = new ConstantBufferParameters::BoundResource[cbInfo.NumberTextures];
				for (unsigned int cb = 0; cb < textures.size(); ++cb)
				{
					cbInfo.Textures[cb] = textures[cb];

#if defined(DUMP_CONSTANTS)
					LogInfo("\tTexture Name: %s", cbInfo.Textures[cb].Name);
					LogInfo("\t\tType: %u", cbInfo.Textures[cb].Type);
					LogInfo("\t\tBind Point: %u", cbInfo.Textures[cb].BindPoint);
					LogInfo("\t\tBind Count: %u", cbInfo.Textures[cb].BindCount);
#endif
				}

				cbInfo.NumberSamplers = (unsigned int)samplers.size();
				cbInfo.Samplers = new ConstantBufferParameters::BoundResource[cbInfo.NumberSamplers];
				for (unsigned int sam = 0; sam < samplers.size(); ++sam)
				{
					cbInfo.Samplers[sam] = samplers[sam];

#if defined(DUMP_CONSTANTS)
					LogInfo("\tSamplers Name: %s", cbInfo.Samplers[sam].Name);
					LogInfo("\t\tType: %u", cbInfo.Samplers[sam].Type);
					LogInfo("\t\tBind Point: %u", cbInfo.Samplers[sam].BindPoint);
					LogInfo("\t\tBind Count: %u", cbInfo.Samplers[sam].BindCount);
#endif
				}
			}

			_pReflection->Release();

			_pShader->SetShaderParameters(shaderIO);
			_pShader->SetConstantParameters(cbInfo);
		}
	}
}
