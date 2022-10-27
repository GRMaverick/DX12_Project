#ifndef __DeviceD3D12_h__
#define __DeviceD3D12_h__

//#include "Aliases.h"

#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include <map>

#include "Helpers\d3dx12.h"

//#include "D3D12\Resources\ConstantBufferParameters.h"

#include "Interfaces/IGraphicsDevice.h"
#include "Interfaces/IConstantBufferParameters.h"
#include "Interfaces/IDescriptorHeap.h"
#include "Interfaces/ISamplerState.h"
#include "Interfaces/ICommandList.h"

using IACommandList = Artemis::Renderer::Interfaces::ICommandList;
using IABufferResource = Artemis::Renderer::Interfaces::IBufferResource;
using EAResourceFlags = Artemis::Renderer::Interfaces::ResourceFlags;

namespace Artemis
{
	namespace Renderer
	{
		namespace Interfaces
		{
			class IBufferResource;
			class ISamplerState;
			class IShaderStage;
			class IBufferResource;
			class IGpuBufferResource;
			class IConstantBufferParameters;
		}
	}
}

namespace Artemis::Core
{
	class GameWindow;
}

namespace Artemis::Renderer::Device::Dx12
{
	class DescriptorHeapDx12;
	class CommandQueueDx12;
	class CommandList;
	class SwapChain;
	class GpuResourceTable;
	class Texture2DResource;
	class VertexBufferResource;
	class IndexBufferResource;
	class ConstantBufferResourceDx12;

	struct SamplerStateEntry;

	struct PipelineStateDesc
	{
		CD3DX12_PIPELINE_STATE_STREAM_VS             VertexShader;
		CD3DX12_PIPELINE_STATE_STREAM_PS             PixelShader;
		CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT   InputLayout;
		CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE RootSignature;
	};

	class DeviceDx12 final : public Artemis::Renderer::Interfaces::IGraphicsDevice
	{
		friend class RendererD3D12;
	public:
		struct DeviceState
		{
			unsigned int             DirtyFlags = 0;
			GpuResourceTable*        Resources;
			D3D12_RASTERIZER_DESC    RasterizerState;
			D3D12_BLEND_DESC         BlendState;
			D3D12_DEPTH_STENCIL_DESC DepthStencilState;

			unsigned int ConstantBufferUpdates = 0;
			unsigned int SamplerStateUpdates   = 0;
			unsigned int TextureUpdates        = 0;
			unsigned int RenderTargetUpdates   = 0;
			unsigned int DepthBufferUpdates    = 0;
			unsigned int RootSignatureUpdates  = 0;
			unsigned int PipelineStateUpdates  = 0;
			unsigned int ShaderUpdates         = 0;

			void SetDirty( const unsigned int _dirtyFlag )
			{
				DirtyFlags |= _dirtyFlag;
			}

			bool IsDirty( const unsigned int _dirtyFlags ) const
			{
				return (DirtyFlags & _dirtyFlags) == _dirtyFlags;
			}
		};

		// IGraphicsDevice Implementation
#pragma region IGraphicsDevice_Implementation
		const void* GetDeviceObject() const override { return m_pDevice.Get(); }
		void        BeginFrame( void ) override;
		void        EndFrame( void ) override;

		bool CreateSwapChain( Interfaces::ISwapChain** _ppSwapChain, Interfaces::ICommandQueue* _pCmdQueue, Core::GameWindow* _pWindow, unsigned int _numBackBuffers, const wchar_t* _pDebugName = L"" ) override;
		bool CreateCommandList( Interfaces::ECommandListType _type, Interfaces::ICommandList** _ppCommandList, const wchar_t* _pDebugName = L"" ) const override;
		bool CreateCommandQueue( Interfaces::ECommandListType _type, Interfaces::ICommandQueue** _ppCommandQueue, const wchar_t* _pDebugName = L"" ) const override;
		bool CreateDescriptorHeap( Interfaces::DescriptorHeapType _type, Interfaces::IDescriptorHeap** _pDescriptorHeap, Interfaces::DescriptorHeapFlags _flags, unsigned int _numBuffers, const wchar_t* _pDebugName = L"" ) const override;

		IABufferResource* CreateVertexBufferResource( IACommandList* _pCommandList, UINT _sizeInBytes, UINT _strideInBytes, EAResourceFlags _flags, const void* _pData, const wchar_t* _pDebugName = L"" ) const override;
		IABufferResource* CreateIndexBufferResource( IACommandList* _pCommandList, UINT _sizeInBytes, UINT _strideInBytes, EAResourceFlags _flags, const void* _pData, const wchar_t* _pDebugName = L"" ) const override;
		IABufferResource* CreateTexture2D( const wchar_t* _pWstrFilename, IACommandList* _pCommandList, const wchar_t* _pDebugName = L"" ) const override;
		IABufferResource* CreateWicTexture2D( const wchar_t* _pWstrFilename, IACommandList* _pCommandList, const wchar_t* _pDebugName = L"" ) const override;

		bool FlushState() override;
		bool SetMaterial( const char* _pName ) override;
		bool SetRenderTarget( void ) override;
		bool SetDepthBuffer( void ) override;
		bool SetTexture( const char* _pName, Interfaces::IGpuBufferResource* _pTexture ) override;
		bool SetConstantBuffer( const char* _pName, Interfaces::IGpuBufferResource* _pCBuffer ) override;
		bool SetSamplerState( const char* _pName, Interfaces::ISamplerState* _pSamplerState ) override;
		bool SetRasterizerState( const Interfaces::RasteriserStateDesc& _desc ) override;
		bool SetBlendState( const Interfaces::BlendDesc& _desc ) override;
		bool SetDepthStencilState( const Interfaces::DepthStencilDesc& _desc ) override;

		Interfaces::ICommandList*  GetImmediateContext( void ) const override { return m_pImmediateContext; }
		Interfaces::ISamplerState* GetDefaultSamplerState( void ) const override { return m_pDefaultSampler; }
#pragma endregion

		DeviceDx12( const bool _bDebugging ) { Initialise( _bDebugging ); }
		~DeviceDx12( void ) override;

		bool Initialise( bool _bDebugging );
		bool InitialiseImGui( HWND _hWindow, const DescriptorHeapDx12* _pSrvHeap ) const;

		Renderer::Interfaces::ISamplerState* CreateSamplerState( Renderer::Interfaces::SamplerStateFilter _eFilter, Renderer::Interfaces::SamplerStateWrapMode _eWrap, Renderer::Interfaces::SamplerStateComparisonFunction _eCompFunc ) const;
		ConstantBufferResourceDx12*          CreateConstantBufferResource( const Renderer::Interfaces::IConstantBufferParameters::ConstantBuffer& _params, const wchar_t* _pDebugName = L"" ) const;

		DeviceState GetDeviceState( void ) const { return m_DeviceState; }
		bool        GetRootSignature( Interfaces::IShaderStage* _pShader, ID3D12RootSignature** _ppRootSignature, const wchar_t* _pDebugName = L"" );
		bool        GetPipelineState( ID3D12PipelineState** _ppPipelineState, const wchar_t* _pDebugName = L"" );
		bool        CreateSamplerState( const D3D12_SAMPLER_DESC* _pSamplerDesc, D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle, const wchar_t* _pDebugName = L"" ) const;

		Interfaces::IDescriptorHeap* GetSrvCbvHeap( void ) const { return m_pDescHeapSrvCbv; }

	private:
		constexpr static unsigned int kDirtyShaders        = 1 << 0;
		constexpr static unsigned int kDirtySamplerState   = 1 << 1;
		constexpr static unsigned int kDirtyRenderTarget   = 1 << 2;
		constexpr static unsigned int kDirtyDepthBuffer    = 1 << 3;
		constexpr static unsigned int kDirtyTexture        = 1 << 4;
		constexpr static unsigned int kDirtyConstantBuffer = 1 << 5;
		constexpr static unsigned int kDirtyPipelineState  = 1 << 6;

		DeviceDx12( void );

		DeviceState m_DeviceState;

		Interfaces::IDescriptorHeap* m_pDescHeapRtv;
		Interfaces::IDescriptorHeap* m_pDescHeapDsv;
		Interfaces::IDescriptorHeap* m_pDescHeapSrvCbv;
		Interfaces::IDescriptorHeap* m_pDescHeapSampler;

		Interfaces::IDescriptorHeap* m_pActiveResourceHeap;
		Interfaces::IDescriptorHeap* m_pActiveSamplerHeap;

		Interfaces::ISamplerState* m_pDefaultSampler;
		D3D12_SAMPLER_DESC         m_eDefaultSampler;

		Interfaces::ICommandList* m_pImmediateContext;

		Microsoft::WRL::ComPtr<ID3D12Device6> m_pDevice = nullptr;

		Microsoft::WRL::ComPtr<IDXGIFactory5> m_pDxgiFactory = nullptr;
		Microsoft::WRL::ComPtr<IDXGIAdapter4> m_pDxgiAdapter = nullptr;

		std::map<unsigned long, GpuResourceTable*> m_mapGpuResourceTables;

		std::map<unsigned int, SamplerStateEntry*>   m_mapSamplers;
		std::map<unsigned int, ID3D12RootSignature*> m_mapRootSignatures;
		std::map<unsigned int, ID3D12PipelineState*> m_mapPso;
	};
}

#endif // __DeviceD3D12_h__
