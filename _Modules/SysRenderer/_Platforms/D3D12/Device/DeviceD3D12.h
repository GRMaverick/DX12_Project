#ifndef __DeviceD3D12_h__
#define __DeviceD3D12_h__

#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include <map>

#include "D3D12\d3dx12.h"

#include "D3D12\Resources\ConstantBufferParameters.h"

#include "_Interfaces\ISamplerState.h"

namespace SysCore
{
	class GameWindow;
}

namespace SysRenderer
{
	namespace Interfaces
	{
		class IShaderStage;
		class IBufferResource;
		class IGpuBufferResource;
	}

	namespace D3D12
	{
		class DescriptorHeap;
		class CommandQueue;
		class CommandList;
		class SwapChain;
		class GpuResourceTable;
		class Texture2DResource;
		class VertexBufferResource;
		class IndexBufferResource;
		class ConstantBufferResource;

		struct SamplerStateEntry;

		struct PipelineStateDesc
		{
			CD3DX12_PIPELINE_STATE_STREAM_VS             VertexShader;
			CD3DX12_PIPELINE_STATE_STREAM_PS             PixelShader;
			CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT   InputLayout;
			CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE RootSignature;
		};

		class DeviceD3D12
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

			~DeviceD3D12( void );

			static DeviceD3D12* Instance( void );

			bool Initialise( bool _bDebugging );
			bool InitialiseImGui( HWND _hWindow, const DescriptorHeap* _pSrvHeap ) const;

			bool CreateCommandQueue( D3D12_COMMAND_LIST_TYPE _type, CommandQueue** _ppCommandQueue, const wchar_t* _pDebugName = L"" ) const;
			bool CreateCommandList( D3D12_COMMAND_LIST_TYPE _type, CommandList** _ppCommandList, const wchar_t* _pDebugName = L"" ) const;
			bool CreateDescriptorHeap( D3D12_DESCRIPTOR_HEAP_TYPE _type, DescriptorHeap** _ppDescriptorHeap, UINT _numBuffers = 1, D3D12_DESCRIPTOR_HEAP_FLAGS _flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE, const wchar_t* _pDebugName = L"" ) const;
			bool CreateSwapChain( SwapChain** _ppSwapChain, SysCore::GameWindow* _pWindow, UINT _numBackBuffers, const wchar_t* _pDebugName = L"" );

			Interfaces::ISamplerState*   CreateSamplerState( SamplerStateFilter _eFilter, SamplerStateWrapMode _eWrap, SamplerStateComparisonFunction _eCompFunc ) const;
			Interfaces::IBufferResource* CreateTexture2D( const wchar_t* _pWstrFilename, CommandList* _pCommandList, const wchar_t* _pDebugName = L"" ) const;
			Interfaces::IBufferResource* CreateWicTexture2D( const wchar_t* _pWstrFilename, CommandList* _pCommandList, const wchar_t* _pDebugName = L"" ) const;
			Interfaces::IBufferResource* CreateIndexBufferResource( CommandList* _pCommandList, UINT _sizeInBytes, UINT _strideInBytes, D3D12_RESOURCE_FLAGS _flags, const void* _pData, const wchar_t* _pDebugName = L"" ) const;
			Interfaces::IBufferResource* CreateVertexBufferResource( CommandList* _pCommandList, UINT _sizeInBytes, UINT _strideInBytes, D3D12_RESOURCE_FLAGS _flags, const void* _pData, const wchar_t* _pDebugName = L"" ) const;
			ConstantBufferResource*      CreateConstantBufferResource( const ConstantBufferParameters::ConstantBuffer& _params, const wchar_t* _pDebugName = L"" ) const;

			DeviceState GetDeviceState( void ) const { return m_DeviceState; }
			bool        GetRootSignature( Interfaces::IShaderStage* _pShader, ID3D12RootSignature** _ppRootSignature, const wchar_t* _pDebugName = L"" );
			bool        GetPipelineState( ID3D12PipelineState** _ppPipelineState, const wchar_t* _pDebugName = L"" );
			bool        CreateSamplerState( const D3D12_SAMPLER_DESC* _pSamplerDesc, D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle, const wchar_t* _pDebugName = L"" ) const;

			bool FlushState();
			bool SetMaterial( const char* _pName );
			bool SetRenderTarget( void );
			bool SetDepthBuffer( void );
			bool SetTexture( const char* _pName, Interfaces::IGpuBufferResource* _pTexture );
			bool SetConstantBuffer( const char* _pName, Interfaces::IGpuBufferResource* _pCBuffer );
			bool SetSamplerState( const char* _pName, Interfaces::ISamplerState* _pSamplerState );
			bool SetRasterizerState( D3D12_RASTERIZER_DESC _desc );
			bool SetBlendState( D3D12_BLEND_DESC _desc );
			bool SetDepthStencilState( D3D12_DEPTH_STENCIL_DESC _desc );

			DescriptorHeap* GetSrvCbvHeap( void ) const { return m_pDescHeapSrvCbv; }

			void BeginFrame( void );
			void EndFrame( void );

			CommandList*               GetImmediateContext( void ) const { return m_pImmediateContext; }
			Interfaces::ISamplerState* GetDefaultSamplerState( void ) const { return m_pDefaultSampler; }

		private:
			constexpr static unsigned int kDirtyShaders        = 1 << 0;
			constexpr static unsigned int kDirtySamplerState   = 1 << 1;
			constexpr static unsigned int kDirtyRenderTarget   = 1 << 2;
			constexpr static unsigned int kDirtyDepthBuffer    = 1 << 3;
			constexpr static unsigned int kDirtyTexture        = 1 << 4;
			constexpr static unsigned int kDirtyConstantBuffer = 1 << 5;
			constexpr static unsigned int kDirtyPipelineState  = 1 << 6;

			DeviceD3D12( void );

			DeviceState m_DeviceState;

			DescriptorHeap* m_pDescHeapRtv;
			DescriptorHeap* m_pDescHeapDsv;
			DescriptorHeap* m_pDescHeapSrvCbv;
			DescriptorHeap* m_pDescHeapSampler;

			DescriptorHeap* m_pActiveResourceHeap;
			DescriptorHeap* m_pActiveSamplerHeap;

			Interfaces::ISamplerState* m_pDefaultSampler;
			D3D12_SAMPLER_DESC         m_eDefaultSampler;

			CommandList* m_pImmediateContext;

			Microsoft::WRL::ComPtr<ID3D12Device6> m_pDevice = nullptr;

			Microsoft::WRL::ComPtr<IDXGIFactory5> m_pDxgiFactory = nullptr;
			Microsoft::WRL::ComPtr<IDXGIAdapter4> m_pDxgiAdapter = nullptr;

			std::map<unsigned long, GpuResourceTable*> m_mapGpuResourceTables;

			std::map<unsigned int, SamplerStateEntry*>   m_mapSamplers;
			std::map<unsigned int, ID3D12RootSignature*> m_mapRootSignatures;
			std::map<unsigned int, ID3D12PipelineState*> m_mapPso;
		};
	}
}

#endif // __DeviceD3D12_h__
