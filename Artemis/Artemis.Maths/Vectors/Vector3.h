#pragma once

namespace Artemis::Maths
{
	class Vector3
	{
	public:
		float x;
		float y;
		float z;

		Vector3( void );
		Vector3( const float& _xyz );
		Vector3( const Vector3& _rhs );
		Vector3( const float& _x, const float& _y, const float& _z );
		~Vector3( void );

		bool IsNormalised( void ) const;

		Vector3 operator*( const float& _scalar ) const;
		Vector3 operator/( const float& _scalar ) const;
		Vector3 operator-( const Vector3& _rhs ) const;
		Vector3 operator+( const Vector3& _rhs ) const;

		void operator-=( const Vector3& _rhs );
		void operator+=( const Vector3& _rhs );
		void operator*=( const Vector3& _rhs );
		void operator/=( const Vector3& _rhs );

		float Magnitude( void ) const;
		float Dot( const Vector3& _rhs ) const;
		float AngleInRadians( const Vector3& _rhs ) const;
		float AngleInDegrees( const Vector3& _rhs ) const;

		Vector3 Negate( void ) const;
		Vector3 Normalise( void ) const;
		Vector3 Cross( const Vector3& _rhs ) const;
	};
}
