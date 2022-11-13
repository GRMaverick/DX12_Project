#pragma once

namespace Artemis::Maths
{
	class Vector4
	{
	public:
		float x;
		float y;
		float z;
		float w;

		Vector4( void );
		Vector4( const float& _xyzw );
		Vector4( const Vector4& _rhs );
		Vector4( const float& _x, const float& _y, const float& _z, const float& _w );
		~Vector4( void );

		Vector4 operator*( const float& _scalar ) const;
		Vector4 operator/( const float& _scalar ) const;
		Vector4 operator-( const Vector4& _rhs ) const;
		Vector4 operator+( const Vector4& _rhs ) const;

		void operator-=( const Vector4& _rhs );
		void operator+=( const Vector4& _rhs );
		void operator*=( const Vector4& _rhs );
		void operator/=( const Vector4& _rhs );

		float Dot( const Vector4& _rhs ) const;
	};
}
