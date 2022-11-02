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

#include "DeviceState.h"

using IACommandList = Artemis::Renderer::Interfaces::ICommandList;
using IAGpuResource = Artemis::Renderer::Interfaces::IGpuResource;
using EAResourceFlags = Artemis::Renderer::Interfaces::ResourceFlags;

namespace Artemis::Renderer::Interfaces
{
	class IBufferResource;
	class ISamplerState;
	class IShaderStage;
	class IBufferResource;
	class IGpuResource;
	class IConstantBufferParameters;
	struct IMaterial;
}

namespace Artemis::Core
{
	class GameWindow;
}

namespace Artemis::Renderer::Device::Dx12
{
	class DescriptorHeapDx12;
	class GpuResourceDx12;

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
		struct SamplerStateEntry
		{
			unsigned int Hash      = 0;
			unsigned int HeapIndex = 0;
		};

		// IGraphicsDevice Implementation
#pragma region IGraphicsDevice_Implementation
		bool InitialiseImGui( Artemis::Core::GameWindow* _pWindow, const Interfaces::IDescriptorHeap* _pSrvHeap ) const override;

		const void* GetDeviceObject() const override { return m_pDevice.Get(); }
		void        BeginFrame( void ) override;
		void        EndFrame( void ) override;

		bool CreateSwapChain( Interfaces::ISwapChain** _ppSwapChain, Interfaces::ICommandQueue* _pCmdQueue, Core::GameWindow* _pWindow, unsigned int _numBackBuffers, const wchar_t* _pDebugName = L"" ) override;
		bool CreateCommandList( Interfaces::ECommandListType _type, Interfaces::ICommandList** _ppCommandList, const wchar_t* _pDebugName = L"" ) const override;
		bool CreateCommandQueue( Interfaces::ECommandListType _type, Interfaces::ICommandQueue** _ppCommandQueue, const wchar_t* _pDebugName = L"" ) const override;
		bool CreateDescriptorHeap( Interfaces::DescriptorHeapType _type, Interfaces::IDescriptorHeap** _pDescriptorHeap, Interfaces::DescriptorHeapFlags _flags, unsigned int _numBuffers, const wchar_t* _pDebugName = L"" ) const override;

		IAGpuResource* CreateVertexBufferResource( IACommandList* _pCommandList, unsigned int _sizeInBytes, unsigned int _strideInBytes, EAResourceFlags _flags, const void* _pData, const wchar_t* _pDebugName = L"" ) const override;
		IAGpuResource* CreateIndexBufferResource( IACommandList* _pCommandList, unsigned int _sizeInBytes, unsigned int _strideInBytes, EAResourceFlags _flags, const void* _pData, const wchar_t* _pDebugName = L"" ) const override;
		IAGpuResource* CreateConstantBufferResource( const Renderer::Interfaces::IConstantBufferParameters::ConstantBuffer& _params, const wchar_t* _pDebugName = L"" ) const override;
		IAGpuResource* CreateTexture2D( const wchar_t* _pWstrFilename, IACommandList* _pCommandList, const wchar_t* _pDebugName = L"" ) const override;
		IAGpuResource* CreateWicTexture2D( const wchar_t* _pWstrFilename, IACommandList* _pCommandList, const wchar_t* _pDebugName = L"" ) const override;
		IAGpuResource* CreateRenderTargetResource( const Interfaces::IGraphicsDevice* _pDevice, const Interfaces::ICommandList* _pCmdList, const unsigned _width, const unsigned _height, const Interfaces::DxgiFormat _format, Interfaces::IDescriptorHeap* _pRtvHeap, Interfaces::IDescriptorHeap* _pSrvHeap ) const override;
		IAGpuResource* CreateDepthBufferResource( const Interfaces::IGraphicsDevice* _pDevice, const Interfaces::ICommandList* _pCmdList, const unsigned _width, const unsigned _height, const Interfaces::DxgiFormat _format, Interfaces::IDescriptorHeap* _pDsvHeap, Interfaces::IDescriptorHeap* _pSrvHeap ) const override;

		bool FlushState() override;
		bool SetMaterial( Interfaces::IMaterial* _pMaterial ) override;
		bool SetRenderTarget( void ) override;
		bool SetDepthBuffer( void ) override;
		bool SetTexture( const char* _pName, IAGpuResource* _pTexture ) override;
		bool SetConstantBuffer( const char* _pName, IAGpuResource* _pCBuffer ) override;
		bool SetSamplerState( const char* _pName, Interfaces::ISamplerState* _pSamplerState ) override;
		bool SetRasterizerState( const Interfaces::RasteriserStateDesc& _desc ) override;
		bool SetBlendState( const Interfaces::BlendDesc& _desc ) override;
		bool SetDepthStencilState( const Interfaces::DepthStencilDesc& _desc ) override;

		Interfaces::ICommandList*  GetImmediateContext( void ) const override { return m_pImmediateContext; }
		Interfaces::ISamplerState* GetDefaultSamplerState( void ) const override { return m_pDefaultSampler; }

		Interfaces::DeviceStateStats GetDeviceStats( void ) const override
		{
			return m_DeviceState.m_stats;
		}
#pragma endregion

		DeviceDx12( const bool _bDebugging ) { Initialise( _bDebugging ); }
		~DeviceDx12( void ) override;

		bool Initialise( bool _bDebugging );
		bool InitialiseImGui( HWND _hWindow, const DescriptorHeapDx12* _pSrvHeap ) const;

		Renderer::Interfaces::ISamplerState* CreateSamplerState( Renderer::Interfaces::SamplerStateFilter _eFilter, Renderer::Interfaces::SamplerStateWrapMode _eWrap, Renderer::Interfaces::SamplerStateComparisonFunction _eCompFunc ) const;

		//DeviceState GetDeviceState( void ) const { return m_DeviceState; }
		bool GetRootSignature( Interfaces::IShaderStage* _pShader, ID3D12RootSignature** _ppRootSignature, const wchar_t* _pDebugName = L"" );
		bool GetPipelineState( ID3D12PipelineState** _ppPipelineState, const wchar_t* _pDebugName = L"" );
		bool CreateSamplerState( const D3D12_SAMPLER_DESC* _pSamplerDesc, D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle, const wchar_t* _pDebugName = L"" ) const;

		Interfaces::IDescriptorHeap* GetSrvCbvHeap( void ) const { return m_pDescHeapSrvCbv; }

	private:
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

		std::map<unsigned int, SamplerStateEntry*>   m_mapSamplers;
		std::map<unsigned int, ID3D12RootSignature*> m_mapRootSignatures;
		std::map<unsigned int, ID3D12PipelineState*> m_mapPso;
	};
}

#endif // __DeviceD3D12_h__
