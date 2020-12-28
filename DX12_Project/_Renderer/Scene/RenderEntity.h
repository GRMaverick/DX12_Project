#ifndef __RenderEntity_h__
#define __RenderEntity_h__

#include <DirectXMath.h>

struct RenderModel;
class ConstantBufferResource;

class RenderEntity
{
public:
	RenderEntity(void)	{}
	~RenderEntity(void) {}

	bool LoadModelFromFile(const char* _pFilename);

	void Update(void);

	void SetPosition(float _x, float _y, float _z)	{ m_Position = DirectX::XMFLOAT3(_x, _y, _z); }
	void SetRotation(float _x, float _y, float _z)	{ m_Rotation = DirectX::XMFLOAT3(_x, _y, _z); }
	void SetScale(float _xyz)						{ m_Scale = _xyz; }
	void SetMaterial(const char* _pMaterialName)	{ m_pMaterialName = _pMaterialName; }
	void SetConstantBuffer(ConstantBufferResource* _pBuffer) { m_pConstantBuffer = _pBuffer; }

	RenderModel*			GetModel(void) const	{ return m_pModel; }
	DirectX::XMFLOAT3		GetPosition(void) const { return m_Position; }
	DirectX::XMFLOAT3		GetRotation(void) const { return m_Rotation; }
	float					GetScale(void) const	{ return m_Scale; }
	DirectX::XMMATRIX		GetWorld(void) const	{ return m_World; }
	const char*				GetMaterialName(void) const { return m_pMaterialName; }

	ConstantBufferResource* GetConstantBuffer(void) const { return m_pConstantBuffer; }
private:
	DirectX::XMFLOAT3		m_Position;
	DirectX::XMFLOAT3		m_Rotation;
	float					m_Scale;

	PRAGMA_TODO("Remove rotation code from Render Entity")
	float					m_RotationTheta; // Remove

	RenderModel*			m_pModel;
	DirectX::XMMATRIX		m_World;

	ConstantBufferResource* m_pConstantBuffer;
	const char* m_pMaterialName = nullptr;
};

#endif