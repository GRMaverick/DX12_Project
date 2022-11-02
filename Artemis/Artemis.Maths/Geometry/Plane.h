#pragma once

//#include "Line.h"
#include "Vectors/Vector3.h"

namespace Artemis::Maths
{
	class Plane
	{
	public:
		Plane( void );
		Plane( const Plane& _rhs );
		Plane( const Vector3& _point, const Vector3& _normal, const Vector3& _extents = Vector3( -1.0f, -1.0f, -1.0f ) );

		void SetPoint( const Vector3& _point );
		void SetNormal( const Vector3& _normal );
		void SetExtents( const Vector3& _extents );

		Vector3 GetPoint( void ) const;
		Vector3 GetNormal( void ) const;
		Vector3 GetExtents( void ) const;

	private:
		Vector3 m_Point;
		Vector3 m_Normal;
		Vector3 m_Extents;
	};
}
