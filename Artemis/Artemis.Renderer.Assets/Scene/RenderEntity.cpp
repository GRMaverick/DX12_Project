#include "Defines.h"

#include "RenderEntity.h"

#include "D3D12\Device\DeviceD3D12.h"
#include "D3D12\Device\CommandList.h"
#include "D3D12\Device\CommandQueue.h"

#include "AssimpLoader.h"

using namespace DirectX;

namespace SysRenderer
{
	using namespace D3D12;
	using namespace Loading;

	namespace Scene
	{
		RenderEntity::RenderEntity():
			m_xm3Position(),
			m_xm3Rotation(),
			m_fScale( 0 ),
			m_RotationTheta( 0 ),
			m_pModel( nullptr ),
			m_matWorld(),
			m_pConstantBuffer( nullptr )
		{
		}

		bool RenderEntity::LoadModelFromFile( const char* _pFilename )
		{
			CommandList* pCmdListCpy = CommandList::Build( D3D12_COMMAND_LIST_TYPE_COPY, L"CopyContext" );
			pCmdListCpy->Reset();

			m_pModel = nullptr;
			if ( !AssimpLoader::LoadModel( DeviceD3D12::Instance(), pCmdListCpy, _pFilename, &m_pModel ) )
				return false;

			CommandQueue::Instance( D3D12_COMMAND_LIST_TYPE_COPY )->SubmitToQueue( pCmdListCpy );
			return true;
		}

		void RenderEntity::Update()
		{
			float sRotationSpeed = 10.0f * 0.016f;
			//m_RotationTheta += sRotationSpeed;

			// Update the model matrix. 
			float          angle    = 25.0f;
			const XMVECTOR rotation = XMVectorSet( 1, 0, 0, 0 );

			const XMMATRIX s = XMMatrixScaling( m_fScale, m_fScale, m_fScale );
			const XMMATRIX r = XMMatrixRotationAxis( rotation, XMConvertToRadians( m_RotationTheta ) );
			const XMMATRIX t = XMMatrixTranslation( m_xm3Position.x, m_xm3Position.y, m_xm3Position.z );
			m_matWorld       = s * r * t;
		}
	}
}
