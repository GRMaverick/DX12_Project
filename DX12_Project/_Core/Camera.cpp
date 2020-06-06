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
	m_View = XMMatrixLookAtLH(m_Position, m_Target, m_Up);
	m_Projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(m_FieldOfView), m_AspectRatio, 0.1f, 100.0f);
}

void Camera::SetUp(float _x, float _y, float _z)
{
	m_Up = XMVectorSet(_x, _y, _z, 1.0f);
}

void Camera::SetTarget(float _x, float _y, float _z)
{
	m_Target = XMVectorSet(_x, _y, _z, 1.0f);
}

void Camera::SetPosition(float _x, float _y, float _z)
{
	m_Position = XMVectorSet(_x, _y, _z, 0.0f);
}

void Camera::SetFieldOfView(float _fov)
{
	m_FieldOfView = _fov;
}
void Camera::SetAspectRatio(float _aspectRatio)
{
	m_AspectRatio = _aspectRatio;
}

XMMATRIX Camera::GetView(void)
{
	return m_View;
}
XMMATRIX Camera::GetProjection(void)
{
	return m_Projection;
}