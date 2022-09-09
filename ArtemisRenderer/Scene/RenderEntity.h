#ifndef __RenderEntity_h__
#define __RenderEntity_h__

#include "Resources/CBStructures.h"

namespace ArtemisRenderer::Resources
{
	class IBufferResource;
	class ConstantBuffer;
}

namespace ArtemisRenderer::Helpers
{
	struct RenderModel;
}

namespace ArtemisRenderer::Scene
{
	class RenderEntity
	{
	public:
		RenderEntity(void) {}
		~RenderEntity(void) {}

		bool LoadModelFromFile(const char* _pFilename);

		void Update(void);

		void SetPosition(float _x, float _y, float _z) { m_Position = DirectX::XMFLOAT3(_x, _y, _z); }
		void SetRotation(float _x, float _y, float _z) { m_Rotation = DirectX::XMFLOAT3(_x, _y, _z); }
		void SetScale(float _xyz) { m_Scale = _xyz; }
		void SetMaterial(const char* _pMaterialName) { m_pMaterialName = _pMaterialName; }
		void SetConstantBuffer(Resources::IBufferResource* _pBuffer) { m_pConstantBuffer = _pBuffer; }
		void SetMaterialData(const Resources::Material& _material) { m_Material = _material; }

		Helpers::RenderModel*		GetModel(void) const { return m_pModel; }
		DirectX::XMFLOAT3			GetPosition(void) const { return m_Position; }
		DirectX::XMFLOAT3			GetRotation(void) const { return m_Rotation; }
		float						GetScale(void) const { return m_Scale; }
		DirectX::XMMATRIX			GetWorld(void) const { return m_World; }
		const char*					GetMaterialName(void) const { return m_pMaterialName; }

		Resources::Material			GetMaterialData(void) const { return m_Material; }
		Resources::ConstantBuffer*	GetConstantBuffer(void) const { return (Resources::ConstantBuffer*)m_pConstantBuffer; }
	private:
		DirectX::XMFLOAT3			m_Position;
		DirectX::XMFLOAT3			m_Rotation;
		float						m_Scale;

		//PRAGMA_TODO("Remove rotation code from Render Entity")
		float						m_RotationTheta; // Remove

		Resources::Material			m_Material;
		Helpers::RenderModel*		m_pModel;
		DirectX::XMMATRIX			m_World;

		Resources::IBufferResource* m_pConstantBuffer;
		const char* m_pMaterialName = nullptr;
	};
}

#endif