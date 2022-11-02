#pragma once

#include <d3d12.h>

#include "Interfaces/IGraphicsDevice.h"
#include "Interfaces/IShader.h"

namespace Artemis::Renderer::Interfaces
{
	class IShaderStage;
	class IGpuResource;
	class ISamplerState;
	struct BlendDesc;
	struct RasteriserStateDesc;
	struct DepthStencilDesc;
}

namespace Artemis::Renderer::Device::Dx12
{
	class ShaderDx12;
	class SamplerStateDx12;
	class GpuResourceDx12;

	class DeviceState
	{
	public:
		constexpr static unsigned int kDirtyShaders        = 1 << 0;
		constexpr static unsigned int kDirtySamplerState   = 1 << 1;
		constexpr static unsigned int kDirtyRenderTarget   = 1 << 2;
		constexpr static unsigned int kDirtyDepthBuffer    = 1 << 3;
		constexpr static unsigned int kDirtyTexture        = 1 << 4;
		constexpr static unsigned int kDirtyConstantBuffer = 1 << 5;
		constexpr static unsigned int kDirtyPipelineState  = 1 << 6;

		bool m_bIsInitialised = false;

		unsigned int DirtyFlags = 0;

		unsigned int m_uiNumberSamplers        = 0;
		unsigned int m_uiNumberTextures        = 0;
		unsigned int m_uiNumberConstantBuffers = 0;

		ShaderDx12* m_pVertexShader = nullptr;
		ShaderDx12* m_pPixelShader  = nullptr;

		SamplerStateDx12** m_pSamplers        = nullptr;
		GpuResourceDx12**  m_pTextures        = nullptr;
		GpuResourceDx12**  m_pConstantBuffers = nullptr;

		D3D12_RASTERIZER_DESC    RasterizerState;
		D3D12_BLEND_DESC         BlendState;
		D3D12_DEPTH_STENCIL_DESC DepthStencilState;

		Interfaces::DeviceStateStats m_stats;

		void Reinitialise( Interfaces::IShaderStage* _pVS, Interfaces::IShaderStage* _pPS );
		void Destroy( void );

		void SetDirty( const unsigned int _dirtyFlag );
		bool SetTexture( const char* _pName, Interfaces::IGpuResource* _pTexture );
		bool SetConstantBuffer( const char* _pName, Interfaces::IGpuResource* _pConstantBuffer );
		bool SetSampler( const char* _pName, Interfaces::ISamplerState* _pSampler );
		bool SetBlendState( const Interfaces::BlendDesc& _desc );
		bool SetRasterizerState( const Interfaces::RasteriserStateDesc& _desc );
		bool SetDepthStencilState( const Interfaces::DepthStencilDesc& _desc );

		unsigned long            GetTextures( GpuResourceDx12*** _ppResources ) const;
		unsigned long            GetConstantBuffers( GpuResourceDx12*** _ppResources ) const;
		unsigned long            GetSamplers( SamplerStateDx12*** _ppResources ) const;
		D3D12_BLEND_DESC         SetBlendState( void );
		D3D12_RASTERIZER_DESC    SetRasterizerState( void );
		D3D12_DEPTH_STENCIL_DESC SetDepthStencilState( void );

		ShaderDx12* GetShader( Interfaces::IShaderStage::ShaderType _type );

		SamplerStateDx12* GetSampler( const char* _pName ) const;
		GpuResourceDx12*  GetTexture( const char* _pName ) const;
		GpuResourceDx12*  GetConstantBuffer( const char* _pName ) const;

		bool IsDirty( const unsigned int _dirtyFlags ) const;
	};
}
