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

		virtual void SetIaPrimitiveTopology( Interfaces::PrimitiveTopology _topology ) const = 0;
		virtual void SetIaVertexBuffers( const unsigned int StartSlot, const unsigned int NumViews, void* _pViews ) const = 0;
		virtual void SetIaIndexBuffer( void* pView ) const = 0;
		virtual void DrawIndexedInstanced( unsigned int _indicesPerInstance, unsigned int _instanceCount, unsigned int _startIndexLocation, unsigned int _baseVertexLocation, unsigned int _startInstanceLocation ) const = 0;

		virtual void ResourceBarrier( const unsigned int _numBarriers, const CD3DX12_RESOURCE_BARRIER* _pBarrier ) const = 0;
		virtual void UpdateSubresource( ID3D12Resource* _pGpu, ID3D12Resource* _pCpu, const unsigned int _intermediateOffset, const unsigned int _firstSubresource, const unsigned int _numSubresources, D3D12_SUBRESOURCE_DATA* _pSubresourceData ) const = 0;
	};
}
