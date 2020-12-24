#include "Defines.h"

#include "Camera.h"

using namespace DirectX;

Camera::Camera(void)
{
	m_View = XMMatrixIdentity();
	m_Projection = XMMatrixIdentity();
}
Camera::~Camera(void)
{
	m_View = XMMatrixIdentity();
	m_Projection = XMMatrixIdentity();
}

void Camera::Update(void)
{
	XMVECTOR vUp = XMVectorSet(m_Up.x, m_Up.y, m_Up.z, 1.0f);
	XMVECTOR vTarget = XMVectorSet(m_Target.x, m_Target.y, m_Target.z, 1.0f);
	XMVECTOR vPosition = XMVectorSet(m_Position.x, m_Position.y, m_Position.z, 1.0f);

	m_View = XMMatrixLookAtLH(vPosition, vTarget, vUp);
	m_Projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(m_FieldOfView), m_AspectRatio, 0.1f, 10000.0f);
}

void Camera::SetUp(float _x, float _y, float _z)
{
	m_Up = XMFLOAT3(_x, _y, _z);
}

void Camera::SetTarget(float _x, float _y, float _z)
{
	m_Target = XMFLOAT3(_x, _y, _z);
}

void Camera::SetPosition(float _x, float _y, float _z)
{
	m_Position = XMFLOAT3(_x, _y, _z);
}

void Camera::SetFieldOfView(float _fov)
{
	m_FieldOfView = _fov;
}
void Camera::SetAspectRatio(float _aspectRatio)
{
	m_AspectRatio = _aspectRatio;
}

XMFLOAT3 Camera::GetUp(void)
{
	return m_Up;
}

XMFLOAT3 Camera::GetTarget(void)
{
	return m_Target;
}

XMFLOAT3 Camera::GetPosition(void)
{
	return m_Position;
}

XMMATRIX Camera::GetView(void)
{
	return m_View;
}

XMMATRIX Camera::GetProjection(void)
{
	return m_Projection;
}