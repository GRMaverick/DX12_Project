#ifndef __CommandList_h__
#define __CommandList_h__

#include <d3d12.h>
#include <wrl.h>

#include "Helpers/d3dx12.h"

#include "Interfaces/ICommandList.h"

#include "DeviceDx12.h"

//#define BREADCRUMB

namespace Artemis::Renderer::Device::Dx12
{
	class ImGUIEngine;

	class CommandListDx12 final : public Renderer::Interfaces::ICommandList
	{
		friend class CommandQueueDx12;
		friend class ImGUIEngine;

	public:
		CommandListDx12( void );
		~CommandListDx12( void ) override;

#pragma region ICommandList Implementation
		bool Initialise( const Interfaces::IGraphicsDevice* _pDevice, Interfaces::ECommandListType _type, const wchar_t* _pDebugName = L"" ) override;
		void SetIaPrimitiveTopology( Interfaces::PrimitiveTopology _topology ) const override;
		void SetDescriptorHeaps( Interfaces::IDescriptorHeap** _ppHeaps, const unsigned int _uiHeaps ) const override;

		void DrawIndexedInstanced( Interfaces::IGpuResource* _pVertexBuffer, Interfaces::IGpuResource* _pIndexBuffer, unsigned int _indices ) const override;
		void DrawImGui() const override;

		virtual void StartMarker(const char* _pFormatString) const override;
		virtual void EndMarker(void) const override;
#pragma endregion

		void Close( void ) const;
		void Reset( void ) const override;

		static void   WriteBreadcrumb( UINT32 _breadcrumb );
		static UINT32 ReadBreadcrumb( void );

		virtual void ResourceBarrier( const UINT32 _numBarriers, const CD3DX12_RESOURCE_BARRIER* _pBarrier ) const override;
		virtual void UpdateSubresource( ID3D12Resource* _pGpu, ID3D12Resource* _pCpu, const UINT _intermediateOffset, const UINT _firstSubresource, const UINT _numSubresources, D3D12_SUBRESOURCE_DATA* _pSubresourceData ) const override;

		void ClearRenderTargetView( D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle, FLOAT _pColor[4], const UINT _numRects, const D3D12_RECT* _pRects ) const;
		void ClearDepthStencilView( const D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle, const D3D12_CLEAR_FLAGS _clearFlags, const FLOAT _depth, const UINT8 _stencil, const UINT _numRects, const D3D12_RECT* _pRects ) const;

		void SetIaVertexBuffers( const unsigned int StartSlot, const unsigned int NumViews, void* _pViews ) const;
		void SetIaIndexBuffer( void* pView ) const;
		void DrawIndexedInstanced( unsigned int _indicesPerInstance, unsigned int _instanceCount, unsigned int _startIndexLocation, unsigned int _baseVertexLocation, unsigned int _startInstanceLocation ) const;

		void SetPipelineState( ID3D12PipelineState* _pPipelineState ) const;
		void SetRsViewports( UINT _numViewports, const D3D12_VIEWPORT* _pViewport ) const;
		void SetRsScissorRects( UINT _numRects, const D3D12_RECT* _pScissorRects ) const;
		void SetOmRenderTargets( const UINT _numRTs, const D3D12_CPU_DESCRIPTOR_HANDLE* _rtCpuDescHandle, const BOOL _bSingleHandleToDescriptor, const D3D12_CPU_DESCRIPTOR_HANDLE* _dsvCpuDescHandle ) const;;
		void SetGraphicsRootSignature( ID3D12RootSignature* _pRootSignature ) const;
		void SetGraphicsRootDescriptorTable( const UINT _rootParameterIndex, const D3D12_GPU_DESCRIPTOR_HANDLE _gpuHandle ) const;
		void SetGraphicsRoot32BitConstants( const UINT _rootParameterIndex, const UINT _num32BitValuesToSet, const void* _pSrcData, const UINT _destOffsetIn32BitValues ) const;
		void SetGraphicsRootConstantBufferView( const UINT _rootParameterIndex, const D3D12_GPU_VIRTUAL_ADDRESS _gpuAddress ) const;
	private:
		bool                    m_bIsRecording;
		char                    m_pName[32];
		D3D12_COMMAND_LIST_TYPE m_clType;

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_pList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator>    m_pAllocator;

#if defined(_DEBUG) && defined(BREADCRUMB)
		UINT32* m_BreadcrumbReadback;
		Microsoft::WRL::ComPtr<ID3D12Resource>				m_BreadcrumbBuffer;
#endif
	};
}

#endif // __CommandList_h__
