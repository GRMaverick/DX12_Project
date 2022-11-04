#include "Camera.h"

__pragma(message("[TODO]: Create Agnostic Maths Library"));
#include <DirectXMath.h>

using namespace DirectX;

namespace Artemis::Renderer::Assets
{
	Camera::Camera( void ) :
		m_xm3Up(),
		m_xm3Target(),
		m_xm3Position()
	{
		m_matView       = XMMatrixIdentity();
		m_matProjection = XMMatrixIdentity();
	}

	Camera::~Camera( void )
	{
		m_matView       = XMMatrixIdentity();
		m_matProjection = XMMatrixIdentity();
	}

	void Camera::Update( void )
	{
		const XMVECTOR vUp       = XMVectorSet( m_xm3Up.x, m_xm3Up.y, m_xm3Up.z, 1.0f );
		const XMVECTOR vTarget   = XMVectorSet( m_xm3Target.x, m_xm3Target.y, m_xm3Target.z, 1.0f );
		const XMVECTOR vPosition = XMVectorSet( m_xm3Position.x, m_xm3Position.y, m_xm3Position.z, 1.0f );

		m_matView       = XMMatrixLookAtLH( vPosition, vTarget, vUp );
		m_matProjection = XMMatrixPerspectiveFovLH( XMConvertToRadians( m_FieldOfView ), m_AspectRatio, 0.1f, 10000.0f );
	}

	void Camera::SetUp( const float _x, const float _y, const float _z )
	{
		m_xm3Up = Artemis::Maths::Vector3( _x, _y, _z );
	}

	void Camera::SetTarget( const float _x, const float _y, const float _z )
	{
		m_xm3Target = Artemis::Maths::Vector3( _x, _y, _z );
	}

	void Camera::SetPosition( const float _x, const float _y, const float _z )
	{
		m_xm3Position = Artemis::Maths::Vector3( _x, _y, _z );
	}

	void Camera::SetFieldOfView( const float _fov )
	{
		m_FieldOfView = _fov;
	}

	void Camera::SetAspectRatio( const float _aspectRatio )
	{
		m_AspectRatio = _aspectRatio;
	}

	Artemis::Maths::Vector3 Camera::GetUp( void ) const
	{
		return m_xm3Up;
	}

	Artemis::Maths::Vector3 Camera::GetTarget( void ) const
	{
		return m_xm3Target;
	}

	Artemis::Maths::Vector3 Camera::GetPosition( void ) const
	{
		return m_xm3Position;
	}

	XMMATRIX Camera::GetView( void ) const
	{
		return m_matView;
	}

	XMMATRIX Camera::GetProjection( void ) const
	{
		return m_matProjection;
	}
}
