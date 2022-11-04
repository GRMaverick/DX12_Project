#pragma once

#include "Vectors/Vector3.h"

namespace Artemis::Maths
{
	class Quaternion
	{
	public:
		float   w;
		Vector3 vector;

		// Default
		Quaternion( void );

		// From Euler
		Quaternion( const Vector3& rhs );

		// Copy
		Quaternion( const Quaternion& _rhs );

		// Vector-Scalar
		Quaternion( const Vector3& _vector, const float& _w );

		// Explicit Component-Wise
		Quaternion( const float& _x, const float& _y, const float& _z, const float& _w );

		~Quaternion( void );

		float Magnitude( void ) const;

		Quaternion& operator+=( const Quaternion& _quat );
		Quaternion& operator-=( const Quaternion& _quat );
		Quaternion& operator*( const Quaternion& quat ) const;
		Quaternion& operator*=( const Quaternion& scalar );

		Quaternion  Conjugate( void ) const;
		Quaternion& Normalise( void ) const;
		Quaternion& RotateByVector( const Vector3& vector );

		void     FromEuler( const Vector3& _vector );
		Vector3& ToEuler( void ) const;
	};
}
