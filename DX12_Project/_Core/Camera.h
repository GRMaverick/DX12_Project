#ifndef __Camera_h__
#define __Camera_h__

#include <DirectXMath.h>

class Camera
{
public:
	Camera();
	~Camera();

	void SetUp(float _x, float _y, float _z);
	void SetTarget(float _x, float _y, float _z);
	void SetPosition(float _x, float _y, float _z);

	void SetFieldOfView(float _fov);
	void SetAspectRatio(float _aspectRatio);
	void Update(void);

	DirectX::XMMATRIX GetView(void);
	DirectX::XMMATRIX GetProjection(void);

private:
	float m_FieldOfView = 0.0f;
	float m_AspectRatio = 0.0f;

	DirectX::XMVECTOR m_Up;
	DirectX::XMVECTOR m_Target;
	DirectX::XMVECTOR m_Position;

	DirectX::XMMATRIX m_View;
	DirectX::XMMATRIX m_Projection;
};

#endif //__Camera_h__