#include "Line.h"

namespace Artemis::Maths
{
	Artemis::Maths::Line::Line( void )
	{
		m_Point     = Vector3();
		m_Direction = Vector3();
	}

	Line::Line( const Line& _rhs )
	{
		m_Point     = _rhs.m_Point;
		m_Direction = _rhs.m_Direction;
	}

	Line::Line( const Vector3& _point, const Vector3& _direction )
	{
		m_Point     = _point;
		m_Direction = _direction;
	}

	void Line::SetPoint( const Vector3& _point )
	{
		m_Point = _point;
	}

	void Line::SetDirection( const Vector3& _direction )
	{
		m_Direction = _direction;
	}

	Vector3 Line::GetPoint( void ) const
	{
		return m_Point;
	}

	Vector3 Line::GetDirection( void ) const
	{
		return m_Direction;
	}

	Vector3 Line::GetPointOnLine( const float& _scalar ) const
	{
		return m_Point + (m_Direction * _scalar);
	}
}
