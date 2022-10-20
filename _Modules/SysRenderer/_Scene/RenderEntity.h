#ifndef __RenderEntity_h__
#define __RenderEntity_h__

#include "CBStructures.h"

namespace SysRenderer
{
	namespace D3D12
	{
		class ConstantBufferResource;
	}

	namespace Loading
	{
		struct RenderModel;
	}

	namespace Scene
	{
		class RenderEntity
		{
		public:
			RenderEntity( void );

			~RenderEntity( void )
			{
			}

			bool LoadModelFromFile( const char* _pFilename );

			void Update( void );

			void SetPosition( const float _x, const float _y, const float _z ) { m_xm3Position = DirectX::XMFLOAT3( _x, _y, _z ); }
			void SetRotation( const float _x, const float _y, const float _z ) { m_xm3Rotation = DirectX::XMFLOAT3( _x, _y, _z ); }
			void SetScale( const float _xyz ) { m_fScale = _xyz; }
			void SetMaterial( const char* _pMaterialName ) { m_pMaterialName = _pMaterialName; }
			void SetConstantBuffer( D3D12::ConstantBufferResource* _pBuffer ) { m_pConstantBuffer = _pBuffer; }
			void SetMaterialData( const Material& _material ) { m_material = _material; }

			Loading::RenderModel* GetModel( void ) const { return m_pModel; }
			DirectX::XMFLOAT3     GetPosition( void ) const { return m_xm3Position; }
			DirectX::XMFLOAT3     GetRotation( void ) const { return m_xm3Rotation; }
			float                 GetScale( void ) const { return m_fScale; }
			DirectX::XMMATRIX     GetWorld( void ) const { return m_matWorld; }
			const char*           GetMaterialName( void ) const { return m_pMaterialName; }

			Material                       GetMaterialData( void ) const { return m_material; }
			D3D12::ConstantBufferResource* GetConstantBuffer( void ) const { return m_pConstantBuffer; }
		private:
			DirectX::XMFLOAT3 m_xm3Position;
			DirectX::XMFLOAT3 m_xm3Rotation;
			float             m_fScale;

			PRAGMA_TODO( "Remove rotation code from Render Entity" )
			float m_RotationTheta; // Remove

			Material              m_material;
			Loading::RenderModel* m_pModel;
			DirectX::XMMATRIX     m_matWorld;

			D3D12::ConstantBufferResource* m_pConstantBuffer;
			const char*                    m_pMaterialName = nullptr;
		};
	}
}

#endif
