#pragma once

#include "Vectors/Vector3.h"

namespace Artemis::Maths
{
	class Matrix3x3
	{
	public:
		Vector3 row1;
		Vector3 row2;
		Vector3 row3;

		Matrix3x3( void );
		Matrix3x3( float _e11, float _e12, float _e13, float _e21, float _e22, float _e23, float _e31, float _e32, float _e33 );
		Matrix3x3( const Matrix3x3& _matrix );
		~Matrix3x3( void );

		Matrix3x3 operator+( const Matrix3x3& _matrix ) const;
		Matrix3x3 operator-( const Matrix3x3& _matrix ) const;
		Matrix3x3 operator*( const Matrix3x3& _matrix ) const;

		Matrix3x3 operator*( const float& _scalar ) const;
		Matrix3x3 operator/( const float& _scalar ) const;

		Matrix3x3& operator+=( const Matrix3x3& _matrix );
		Matrix3x3& operator-=( const Matrix3x3& _matrix );
		Matrix3x3& operator*=( const float& _scalar );
		Matrix3x3& operator/=( const float& _scalar );

		float     Determinant( void ) const;
		Matrix3x3 Transpose( void ) const;
		Matrix3x3 Inverse( void ) const;

		Vector3 GetColumn( const unsigned int _index ) const;

		static Vector3& VectorMultiply( const Vector3& _vector, const Matrix3x3& _matrix );
	};
}
