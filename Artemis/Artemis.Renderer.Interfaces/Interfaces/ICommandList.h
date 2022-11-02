#pragma once
#include "IGraphicsDevice.h"

struct CD3DX12_RESOURCE_BARRIER;
struct ID3D12Resource;
struct D3D12_SUBRESOURCE_DATA;

namespace Artemis::Renderer::Interfaces
{
	enum PrimitiveTopology
	{
		PrimitiveTopology_Undefined = 0,
		PrimitiveTopology_Point = 1,
		PrimitiveTopology_Line = 2,
		PrimitiveTopology_Triangle = 3,
		PrimitiveTopology_Patch = 4,
		PrimitiveTopology_TriangleList = PrimitiveTopology_Patch,
	};

	class ICommandList
	{
	public:
		virtual ~ICommandList( void );

		virtual bool Initialise( const IGraphicsDevice* _pDevice, Interfaces::ECommandListType _type, const wchar_t* _pDebugName = L"" ) = 0;
		virtual void StartMarker( const char* _pFormatString ) const = 0;
		virtual void EndMarker( void ) const = 0;
		virtual void Reset( void ) const = 0;

		virtual void SetIaPrimitiveTopology( PrimitiveTopology _topology ) const = 0;
		virtual void SetDescriptorHeaps( IDescriptorHeap** _ppHeaps, const unsigned int _uiHeaps ) const = 0;
		virtual void DrawIndexedInstanced( IGpuResource* _pVertexBuffer, IGpuResource* _pIndexBuffer, unsigned int _indices ) const = 0;
		virtual void DrawImGui( void ) const = 0;

		virtual void ResourceBarrier( const unsigned int _numBarriers, const CD3DX12_RESOURCE_BARRIER* _pBarrier ) const = 0;
		virtual void UpdateSubresource( ID3D12Resource* _pGpu, ID3D12Resource* _pCpu, const unsigned int _intermediateOffset, const unsigned int _firstSubresource, const unsigned int _numSubresources, D3D12_SUBRESOURCE_DATA* _pSubresourceData ) const = 0;
	};
}
