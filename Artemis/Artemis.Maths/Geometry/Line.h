#pragma once

#include "Vectors/Vector3.h"

namespace Artemis::Maths
{
	class Line
	{
	public:
		Line( void );
		Line( const Line& _rhs );
		Line( const Vector3& _point, const Vector3& _direction );

		void SetPoint( const Vector3& _point );
		void SetDirection( const Vector3& _direction );

		Vector3 GetPoint( void ) const;
		Vector3 GetDirection( void ) const;
		Vector3 GetPointOnLine( const float& _scalar ) const;

	private:
		Vector3 m_Point;
		Vector3 m_Direction;
	};
}
