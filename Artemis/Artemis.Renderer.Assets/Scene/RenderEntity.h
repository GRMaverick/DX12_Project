#ifndef __RenderEntity_h__
#define __RenderEntity_h__

#include <stdio.h>
#include <Windows.h>

#include "CBStructures.h"

namespace Artemis::Renderer::Interfaces
{
    class IGpuResource;
    class IGraphicsDevice;
    class ICommandList;
}

namespace Artemis::Renderer::Assets
{
	struct RenderModel;

	class RenderEntity
	{
	public:
		RenderEntity( void );

		~RenderEntity( void )
		{
		}

		bool LoadModelFromFile( const char* _pFilename, Interfaces::IGraphicsDevice* _pDevice, Interfaces::ICommandList* _pCmdList);

		void Update( void );

		void SetPosition( const float _x, const float _y, const float _z ) { m_xm3Position = DirectX::XMFLOAT3( _x, _y, _z ); }
		void SetRotation( const float _x, const float _y, const float _z ) { m_xm3Rotation = DirectX::XMFLOAT3( _x, _y, _z ); }
		void SetScale( const float _xyz ) { m_fScale = _xyz; }
		void SetMaterial( const char* _pMaterialName ) { _snprintf_s( m_pMaterialName, ARRAYSIZE( m_pMaterialName ), "%s", _pMaterialName ); }
		void SetConstantBuffer( Interfaces::IGpuResource* _pBuffer ) { m_pConstantBuffer = _pBuffer; }
		void SetMaterialData( const Material& _material ) { m_material = _material; }
		void SetModelName( const char* _pName ) { _snprintf_s( m_pModelName, ARRAYSIZE( m_pModelName ), "%s", _pName ); }

		RenderModel*      GetModel( void ) const { return m_pModel; }
		DirectX::XMFLOAT3 GetPosition( void ) const { return m_xm3Position; }
		DirectX::XMFLOAT3 GetRotation( void ) const { return m_xm3Rotation; }
		float             GetScale( void ) const { return m_fScale; }
		DirectX::XMMATRIX GetWorld( void ) const { return m_matWorld; }
		const char*       GetMaterialName( void ) const { return m_pMaterialName; }
		const char*       GetModelName( void ) const { return m_pModelName; }

		Material                  GetMaterialData( void ) const { return m_material; }
		Interfaces::IGpuResource* GetConstantBuffer( void ) const { return m_pConstantBuffer; }
	private:
		DirectX::XMFLOAT3 m_xm3Position;
		DirectX::XMFLOAT3 m_xm3Rotation;
		float             m_fScale;

		//PRAGMA_TODO("Remove rotation code from Render Entity")
		float m_RotationTheta; // Remove

		Material          m_material;
		RenderModel*      m_pModel;
		DirectX::XMMATRIX m_matWorld;

		Interfaces::IGpuResource* m_pConstantBuffer   = nullptr;
		char                      m_pMaterialName[32] = {};
		char                      m_pModelName[32]    = {};
	};
}

#endif
