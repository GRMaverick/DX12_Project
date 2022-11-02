#include "Plane.h"

namespace Artemis::Maths
{
	Plane::Plane( void )
	{
		this->m_Point   = Vector3( 0.0f );
		this->m_Normal  = Vector3( 0.0f );
		this->m_Extents = Vector3( 0.0f );
	}

	Plane::Plane( const Plane& _rhs )
	{
		this->m_Point   = _rhs.m_Point;
		this->m_Normal  = _rhs.m_Normal;
		this->m_Extents = _rhs.m_Extents;
	}

	Plane::Plane( const Vector3& _point, const Vector3& _normal, const Vector3& _extents )
	{
		this->m_Point   = _point;
		this->m_Normal  = _normal;
		this->m_Extents = _extents;
	}

	void Plane::SetPoint( const Vector3& _point )
	{
		m_Point = _point;
	}

	void Plane::SetNormal( const Vector3& _normal )
	{
		m_Normal = _normal;
	}

	void Plane::SetExtents( const Vector3& _extents )
	{
		m_Extents = _extents;
	}

	Vector3 Plane::GetPoint( void ) const
	{
		return m_Point;
	}

	Vector3 Plane::GetNormal( void ) const
	{
		return m_Normal;
	}

	Vector3 Plane::GetExtents( void ) const
	{
		return m_Extents;
	}
}
