#pragma once

#include "Vectors/Vector3.h"

namespace Artemis::Maths
{
	class Matrix3x3
	{
	public:
		float m_row1[3];
		float m_row2[3];
		float m_row3[3];

		Matrix3x3( void );
		Matrix3x3( float _e11, float _e12, float _e13, float _e21, float _e22, float _e23, float _e31, float _e32, float _e33 );
		Matrix3x3( const Matrix3x3& _matrix );
		~Matrix3x3( void );

		Matrix3x3 operator+( const Matrix3x3& _matrix );
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

		static Vector3& VectorMultiply( const Vector3& _vector, const Matrix3x3& _matrix );
	};
}
