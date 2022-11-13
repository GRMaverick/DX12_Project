#pragma once

#include "Matrix3x3.h"
#include "Vectors/Vector4.h"

namespace Artemis::Maths
{
	class Matrix4x4
	{
	public:
		Vector4 row1;
		Vector4 row2;
		Vector4 row3;
		Vector4 row4;

		Matrix4x4( void );
		Matrix4x4( float _e11, float _e12, float _e13, float _e14, float _e21, float _e22, float _e23, float _e24, float _e31, float _e32, float _e33, float _e34, float _e41, float _e42, float _e43, float _e44 );
		Matrix4x4( const Matrix4x4& _matrix );
		~Matrix4x4( void );

		Matrix4x4 operator+( const Matrix4x4& _matrix ) const;
		Matrix4x4 operator-( const Matrix4x4& _matrix ) const;
		Matrix4x4 operator*( const Matrix4x4& _matrix ) const;

		Matrix4x4 operator*( const float& _scalar ) const;
		Matrix4x4 operator/( const float& _scalar ) const;

		Matrix4x4& operator+=( const Matrix4x4& _matrix );
		Matrix4x4& operator-=( const Matrix4x4& _matrix );
		Matrix4x4& operator*=( const float& _scalar );
		Matrix4x4& operator/=( const float& _scalar );

		float     Determinant( void ) const;
		Matrix4x4 Transpose( void ) const;
		Matrix4x4 Inverse( void ) const;

		Vector4 GetColumn( const unsigned int& _index ) const;

		static Vector4 VectorMultiply( const Vector4& _vector, const Matrix4x4& _matrix );

	private:
		Matrix4x4 CofactorMatrix( Matrix4x4 _src );
		Matrix4x4 Adjugate( void ) const;
	};

	Matrix4x4 operator*( const float& _scalar, const Matrix4x4& _matrix );
}
