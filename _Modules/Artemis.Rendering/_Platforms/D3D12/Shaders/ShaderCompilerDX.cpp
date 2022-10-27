#include "Defines.h"

#include "IShader.h"

#include "ShaderCompilerDX.h"

#include <d3dcompiler.h>

#include "D3D12\Resources\ConstantBufferParameters.h"

//#define DUMP_CONSTANTS

using namespace SysUtilities;

namespace SysRenderer
{
	using namespace Interfaces;

	namespace D3D12
	{
		void ShaderCompilerDX::ReflectInternal( IShaderStage* _pShader, ID3D12ShaderReflection* _pReflection ) const
		{
			if ( _pReflection )
			{
				D3D12_SHADER_DESC desc{};
				VALIDATE_D3D( _pReflection->GetDesc(&desc) );

				ShaderIoParameters shaderIo;
				shaderIo.NumberInputs = desc.InputParameters;
				shaderIo.Inputs       = new ShaderIoParameters::Parameter[shaderIo.NumberInputs];
				for ( unsigned int i = 0; i < shaderIo.NumberInputs; ++i )
				{
					D3D12_SIGNATURE_PARAMETER_DESC paramDesc{};
					VALIDATE_D3D( _pReflection->GetInputParameterDesc(i, &paramDesc) );

					ShaderIoParameters::Parameter& p = shaderIo.Inputs[i];
					p.Register                       = paramDesc.Register;
					p.ComponentType                  = paramDesc.ComponentType;
					p.SemanticIndex                  = paramDesc.SemanticIndex;
					p.SystemValueType                = paramDesc.SystemValueType;
					p.Mask                           = paramDesc.Mask;
					strncpy_s( p.SemanticName, paramDesc.SemanticName, ARRAYSIZE( p.SemanticName ) );
				}

				shaderIo.NumberOutputs = desc.OutputParameters;
				shaderIo.Outputs       = new ShaderIoParameters::Parameter[shaderIo.NumberOutputs];
				for ( unsigned int i = 0; i < shaderIo.NumberOutputs; ++i )
				{
					D3D12_SIGNATURE_PARAMETER_DESC paramDesc{};
					VALIDATE_D3D( _pReflection->GetOutputParameterDesc(i, &paramDesc) );

					ShaderIoParameters::Parameter& p = shaderIo.Outputs[i];
					p.Register                       = paramDesc.Register;
					p.ComponentType                  = paramDesc.ComponentType;
					p.SemanticIndex                  = paramDesc.SemanticIndex;
					p.SystemValueType                = paramDesc.SystemValueType;
					p.Mask                           = paramDesc.Mask;
					strncpy_s( p.SemanticName, paramDesc.SemanticName, ARRAYSIZE( p.SemanticName ) );
				}

				ConstantBufferParameters cbInfo;
				cbInfo.NumberBuffers = desc.ConstantBuffers;
				if ( cbInfo.NumberBuffers )
				{
					cbInfo.Buffers = new ConstantBufferParameters::ConstantBuffer[cbInfo.NumberBuffers];
					for ( unsigned int cb = 0; cb < cbInfo.NumberBuffers; ++cb )
					{
						ID3D12ShaderReflectionConstantBuffer* pCBuffer = _pReflection->GetConstantBufferByIndex( cb );
						if ( pCBuffer )
						{
							D3D12_SHADER_BUFFER_DESC sbDesc;
							VALIDATE_D3D( pCBuffer->GetDesc(&sbDesc) );

#if defined(DUMP_CONSTANTS)
							LogInfo( "CB Name: %s", sbDesc.Name );
							LogInfo( "\tSize: %u", sbDesc.Size );
							LogInfo( "\tType: %u", sbDesc.Type );
							LogInfo( "\tVariables: %u", sbDesc.Variables );
#endif

							ConstantBufferParameters::ConstantBuffer& rCBuffer = cbInfo.Buffers[cb];
							strncpy_s( rCBuffer.Name, sbDesc.Name, ARRAYSIZE( rCBuffer.Name ) );
							rCBuffer.Size            = sbDesc.Size;
							rCBuffer.Type            = sbDesc.Type;
							rCBuffer.NumberVariables = sbDesc.Variables;

							if ( rCBuffer.NumberVariables )
							{
								rCBuffer.Variables = new ConstantBufferParameters::Variable[rCBuffer.NumberVariables];
								for ( unsigned int var = 0; var < sbDesc.Variables; ++var )
								{
									ID3D12ShaderReflectionVariable* pCBVariable = pCBuffer->GetVariableByIndex( var );
									if ( pCBVariable )
									{
										D3D12_SHADER_VARIABLE_DESC varDesc;
										VALIDATE_D3D( pCBVariable->GetDesc(&varDesc) );

										ID3D12ShaderReflectionType* pType = pCBVariable->GetType();
										D3D12_SHADER_TYPE_DESC      typeDesc;
										VALIDATE_D3D( pType->GetDesc(&typeDesc) );

										ConstantBufferParameters::Variable& rVariable = rCBuffer.Variables[var];
										strncpy_s( rVariable.Name, varDesc.Name, ARRAYSIZE( rVariable.Name ) );
										rVariable.Size   = varDesc.Size;
										rVariable.Offset = varDesc.StartOffset;

#if defined(DUMP_CONSTANTS)
										LogInfo( "\tVar Name: %s", rVariable.Name );
										LogInfo( "\t\tSize: %u", rVariable.Size );
										LogInfo( "\t\tOffset: %u", rVariable.Offset );
#endif
									}
								}
							}
						}
					}
				}

				if ( desc.BoundResources )
				{
					std::vector<ConstantBufferParameters::BoundResource> textures;
					std::vector<ConstantBufferParameters::BoundResource> samplers;

					for ( unsigned int br = 0; br < desc.BoundResources; ++br )
					{
						D3D12_SHADER_INPUT_BIND_DESC ibDesc;
						VALIDATE_D3D( _pReflection->GetResourceBindingDesc(br, &ibDesc) );

						ConstantBufferParameters::BoundResource resource;
						strncpy_s( resource.Name, ibDesc.Name, ARRAYSIZE( resource.Name ) );
						resource.Type      = ibDesc.Type;
						resource.BindPoint = ibDesc.BindPoint;
						resource.BindCount = ibDesc.BindCount;

						switch ( resource.Type )
						{
							case D3D_SIT_CBUFFER:
								for ( unsigned int cb = 0; cb < cbInfo.NumberBuffers; ++cb )
								{
									if ( strncmp( cbInfo.Buffers[cb].Name, resource.Name, ARRAYSIZE( cbInfo.Buffers[cb].Name ) ) == 0 )
									{
										cbInfo.Buffers[cb].BindPoint = resource.BindPoint;
									}
								}
								break;
							case D3D_SIT_TEXTURE:
								textures.push_back( resource );
								break;
							case D3D_SIT_SAMPLER:
								samplers.push_back( resource );
								break;
						}
					}

					cbInfo.NumberTextures = static_cast<unsigned int>(textures.size());
					cbInfo.Textures       = new ConstantBufferParameters::BoundResource[cbInfo.NumberTextures];
					for ( unsigned int cb = 0; cb < textures.size(); ++cb )
					{
						cbInfo.Textures[cb] = textures[cb];

#if defined(DUMP_CONSTANTS)
						LogInfo( "\tTexture Name: %s", cbInfo.Textures[cb].Name );
						LogInfo( "\t\tType: %u", cbInfo.Textures[cb].Type );
						LogInfo( "\t\tBind Point: %u", cbInfo.Textures[cb].BindPoint );
						LogInfo( "\t\tBind Count: %u", cbInfo.Textures[cb].BindCount );
#endif
					}

					cbInfo.NumberSamplers = static_cast<unsigned int>(samplers.size());
					cbInfo.Samplers       = new ConstantBufferParameters::BoundResource[cbInfo.NumberSamplers];
					for ( unsigned int sam = 0; sam < samplers.size(); ++sam )
					{
						cbInfo.Samplers[sam] = samplers[sam];

#if defined(DUMP_CONSTANTS)
						LogInfo( "\tSamplers Name: %s", cbInfo.Samplers[sam].Name );
						LogInfo( "\t\tType: %u", cbInfo.Samplers[sam].Type );
						LogInfo( "\t\tBind Point: %u", cbInfo.Samplers[sam].BindPoint );
						LogInfo( "\t\tBind Count: %u", cbInfo.Samplers[sam].BindCount );
#endif
					}
				}

				_pReflection->Release();

				_pShader->SetShaderParameters( shaderIo );
				_pShader->SetConstantParameters( cbInfo );
			}
		}
	}
}
