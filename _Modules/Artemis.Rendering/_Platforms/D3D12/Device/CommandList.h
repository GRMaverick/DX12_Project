#ifndef __CommandList_h__
#define __CommandList_h__

#include <d3d12.h>
#include <wrl.h>

#include "D3D12\d3dx12.h"

#include "DeviceD3D12.h"

#include "pix3.h"

//#define BREADCRUMB

namespace SysRenderer
{
	namespace ImGuiUtils
	{
		class ImGUIEngine;
	}

	namespace D3D12
	{
		class CommandList
		{
			friend class CommandQueue;
			friend class ImGuiUtils::ImGUIEngine;

		public:
			CommandList( void );
			~CommandList( void );

			static CommandList* Build( D3D12_COMMAND_LIST_TYPE _type, const wchar_t* _pDebugName = L"" )
			{
				CommandList* pCommandList = nullptr;
				if ( !DeviceD3D12::Instance()->CreateCommandList( _type, &pCommandList, _pDebugName ) )
					return nullptr;
				return pCommandList;
			}

			bool Initialise( Microsoft::WRL::ComPtr<ID3D12Device> _pDevice, D3D12_COMMAND_LIST_TYPE _type, const wchar_t* _pDebugName = L"" );

			void Close( void ) const;
			void Reset( void ) const;

			static void   WriteBreadcrumb( UINT32 _breadcrumb );
			static UINT32 ReadBreadcrumb( void );

			void ResourceBarrier( const UINT32 _numBarriers, const CD3DX12_RESOURCE_BARRIER* _pBarrier ) const;
			void UpdateSubresource( ID3D12Resource* _pGpu, ID3D12Resource* _pCpu, const UINT _intermediateOffset, const UINT _firstSubresource, const UINT _numSubresources, D3D12_SUBRESOURCE_DATA* _pSubresourceData ) const;

			void ClearRenderTargetView( D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle, FLOAT _pColor[4], const UINT _numRects, const D3D12_RECT* _pRects ) const;
			void ClearDepthStencilView( const D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle, const D3D12_CLEAR_FLAGS _clearFlags, const FLOAT _depth, const UINT8 _stencil, const UINT _numRects, const D3D12_RECT* _pRects ) const;

			void SetPipelineState( ID3D12PipelineState* _pPipelineState ) const;
			void SetRsViewports( UINT _numViewports, const D3D12_VIEWPORT* _pViewport ) const;
			void SetRsScissorRects( UINT _numRects, const D3D12_RECT* _pScissorRects ) const;
			void SetIaPrimitiveTopology( D3D12_PRIMITIVE_TOPOLOGY _topology ) const;
			void SetIaVertexBuffers( const UINT StartSlot, const UINT NumViews, const D3D12_VERTEX_BUFFER_VIEW* _pViews ) const;
			void SetIaIndexBuffer( const D3D12_INDEX_BUFFER_VIEW* pView ) const;
			void SetOmRenderTargets( const UINT _numRTs, const D3D12_CPU_DESCRIPTOR_HANDLE* _rtCpuDescHandle, const BOOL _bSingleHandleToDescriptor, const D3D12_CPU_DESCRIPTOR_HANDLE* _dsvCpuDescHandle ) const;

			void SetDescriptorHeaps( ID3D12DescriptorHeap* const* _pHeaps, UINT _numHeaps ) const;
			void SetGraphicsRootSignature( ID3D12RootSignature* _pRootSignature ) const;
			void SetGraphicsRootDescriptorTable( const UINT _rootParameterIndex, const D3D12_GPU_DESCRIPTOR_HANDLE _gpuHandle ) const;
			void SetGraphicsRoot32BitConstants( const UINT _rootParameterIndex, const UINT _num32BitValuesToSet, const void* _pSrcData, const UINT _destOffsetIn32BitValues ) const;
			void SetGraphicsRootConstantBufferView( const UINT _rootParameterIndex, const D3D12_GPU_VIRTUAL_ADDRESS _gpuAddress ) const;

			void DrawImGui() const;
			void DrawIndexedInstanced( UINT _indicesPerInstance, UINT _instanceCount, UINT _startIndexLocation, UINT _baseVertexLocation, UINT _startInstanceLocation ) const;

			template <typename... Args>
			void StartMarker( const char* _pFormatString, Args ..._args ) const
			{
				// CPU
				PIXBeginEvent( PIX_COLOR_DEFAULT, _pFormatString, _args... );

				// GPU
				PIXBeginEvent( m_pList.Get(), PIX_COLOR_INDEX( 0 ), _pFormatString, _args... );
			}

			void EndMarker( void ) const
			{
				// CPU
				PIXEndEvent();

				// GPU
				PIXEndEvent( m_pList.Get() );
			}

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
}

#endif // __CommandList_h__
